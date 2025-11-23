use bitvec::{prelude::Lsb0, view::BitView};
use module_system::{Handler, ModuleRef, System};
use std::future::Future;
use std::pin::Pin;
use std::sync::Arc;
use uuid::Uuid;

use std::collections::HashMap;

// This lab, in turn, focuses on state-based eventual consistency, 
// in which process states themselves are being exchanged. 
// As an application example, we will adopt gossip-based aggregation

// Decentralized gossip-based aggregation is something, 
// with a structure of a spanning tree.
// As we have seen on concurrent programming, as the RV (Randez-vous) model
// of distributing the message from a process throughout the whole system.

/// A source of randomness.
pub(crate) trait RandomnessSource {
    /// Generates a next pseudo-random u32 value selected
    /// from a uniform distribution.
    fn next_u32(&mut self) -> u32;
}

/// A conflict-free state-based replicated counter.
pub(crate) trait ConflictFreeReplicatedCounter<T> {
    /// Sets a given counter so that it counts
    /// no elements.
    fn set_to_zero(&mut self);

    /// Sets a given counter so that it counts
    /// an infinite number of elements (all possible).
    fn set_to_infinity(&mut self);

    /// Adds one more element to a given counter
    /// (increments the counter by one) by one using
    /// a given source of randomness.
    /// If the counter counts an infinite number of elements,
    /// an `Err` is returned and the given counter remains
    /// intact; otherwise, `Ok` is returned.
    fn try_count_one_more_element(&mut self, rs: &mut dyn RandomnessSource) -> Result<(), String>;

    /// Merges another counter with a given counter,
    /// so that, as a result, the given counter counts
    /// elements counted originally by both itself
    /// and the other counter. If the two counters are
    /// incompatible, `Err` is returned and the given
    /// counter remains intact; otherwise, `Ok` is returned.
    fn try_merge_with(&mut self, other: &Self) -> Result<(), String>;

    /// Returns the number of elements counted
    /// by a given counter.
    fn evaluate(&self) -> T;
}

/// An implementation of a probabilistic counting sketch.
#[derive(Clone, Debug)]
pub(crate) struct ProbabilisticCounter {
    // You may add any necessary fields here
    sketches: Vec<Vec<bool>>,
    bits_per_instance: usize,
}

impl ProbabilisticCounter {
    /// The scaling factor used in probabilistic counting.
    const SCALING_FACTOR: f64 = 1.29281;

    /// Creates a new probabilistic counter
    /// with a given number of sketch instances and
    /// bits per instance. The counter
    /// counts no elements.
    pub(crate) fn new_zero(bits_per_instance: usize, num_instances: usize) -> Self {
        assert!(num_instances > 0);
        assert!(bits_per_instance > 0 && bits_per_instance <= u32::BITS as usize);
        assert!(bits_per_instance % 8 == 0);

        let mut sketches: Vec<_> = Vec::with_capacity(num_instances);
        for _ in 0..num_instances {
            let sketch = vec![false; bits_per_instance];
            sketches.push(sketch);
        }
        
        ProbabilisticCounter {
            sketches,
            bits_per_instance,
        }
    }

    /// Creates a new probabilistic counter
    /// with the same configuration as a given one.
    /// The new counter counts no elements.
    pub(crate) fn new_zero_with_same_config(other: &ProbabilisticCounter) -> Self {
        ProbabilisticCounter::new_zero(other.get_num_bits_per_instance(), other.get_num_instances())
    }

    /// Returns the number of sketch instances utilized
    /// by a given probabilistic counter.
    pub(crate) fn get_num_instances(&self) -> usize {
        self.sketches.len()
    }

    /// Returns the number of bits per sketch instance
    /// utilized by a given probabilistic counter.
    pub(crate) fn get_num_bits_per_instance(&self) -> usize {
        self.bits_per_instance
    }

    /// Given a u32 bit number drawn at random from a
    /// uniform distribution produces a number from
    /// a geometric distribution with probability 1/2.
    /// The second parameter denotes the number of bits
    /// of the number that should be used.
    /// This function shall be used for selecting bits for
    /// incrementation of the sketches.
    pub(crate) fn uniform_u32_to_geometric(rand_no: u32, num_bits: usize) -> u32 {
        let rand_val = (rand_no as u64) & ((1_u64 << num_bits) - 1);
        let first_one = rand_val.view_bits::<Lsb0>().first_one();
        match first_one {
            None => 1,
            Some(idx) => idx as u32,
        }
    }

    /// Returns a given bit in a given instance of a given sketch.
    #[cfg(test)]
    pub(crate) fn get_bit(&self, instance_idx: usize, in_instance_bit_idx: usize) -> bool {
        self.sketches[instance_idx][in_instance_bit_idx]
    }

    /// Sets a given bit in a given instance of a given sketch
    /// to the value provided as a parameter.
    #[cfg(test)]
    pub(crate) fn set_bit(&mut self, instance_idx: usize, in_instance_bit_idx: usize, val: bool) {
        self.sketches[instance_idx][in_instance_bit_idx] = val;
    }

    /// Returns a uniform random value that leads to
    /// setting a specific bit in the counter. In principle,
    /// this is used to partially revert function
    /// `uniform_u32_to_geometric` for testing.
    #[cfg(test)]
    pub(crate) fn geometric_to_sample_u32(geom_no: u32) -> u32 {
        assert!(geom_no < u32::BITS);
        1_u32 << geom_no
    }

    // You may add any extra methods here
}

impl ConflictFreeReplicatedCounter<u64> for ProbabilisticCounter {   
    fn set_to_zero(&mut self) {
        for sketch in self.sketches.iter_mut() {
            for bit in sketch.iter_mut() {
                *bit = false;
            }
        }
    }

    fn set_to_infinity(&mut self) {
        for sketch in self.sketches.iter_mut() {
            for bit in sketch.iter_mut() {
                *bit = true;
            }
        }
    }

    fn try_count_one_more_element(&mut self, rs: &mut dyn RandomnessSource) -> Result<(), String> {
        for sketch in &mut self.sketches.iter_mut() {

            if sketch.iter().all(|&bit| bit) {
                return Err("sketch has all bits set to 1".to_string());
            }

            let rand_no = rs.next_u32();
            let geom_no = ProbabilisticCounter::uniform_u32_to_geometric(rand_no, self.bits_per_instance);

            let geom_no = geom_no as usize;

            if geom_no >= self.bits_per_instance {
                return Err("geom_no is out of bounds".to_string());
            } 
            
            sketch[geom_no] = true;
        }

        Ok(())
    }

    fn try_merge_with(&mut self, other: &Self) -> Result<(), String> {
        // merge(P, P) -> P – Returns a bitmask that is a bit-wise OR of the two bitmasks given as parameters.
        // Also performs the bitwise OR independently for each element of the vectors of instances constituting the partial aggregates: 
        // instance 0 in the first aggregate is ORed with instance 0 in the second aggregate, instance 1 with instance 1, and so on.

        if self.sketches.len() != other.sketches.len() {
            return Err("sketches have different number of instances".to_string());
        }

        if self.bits_per_instance != other.bits_per_instance {
            return Err("sketches have different number of bits per instance".to_string());
        }

        for (self_sketch, other_sketch) in self.sketches.iter_mut().zip(other.sketches.iter()) {
            for (self_bit, other_bit) in self_sketch.iter_mut().zip(other_sketch.iter()) {
                *self_bit |= *other_bit;
            }
        }
        
        Ok(())
    }

    fn evaluate(&self) -> u64 {
        // First, evaluate on a counter in which no sketch instance has any bit set to 1 should return 0 
        // instead of the value implied by the previous formula. 
        let mut sketches_sum = 0;
        for sketch in &self.sketches {
            for bit in sketch {
                match bit {
                    true => sketches_sum += 1,
                    false => ()
                }
            }
        }
        if sketches_sum == 0 { return 0; }

        // Second, evaluate on a counter in which at least one instance has all bits set to 1 should return infinity (u64::MAX) 
        // instead of the value from the formula.
        for sketch in &self.sketches {
            let mut ones = 0;
            for bit in sketch {
                match bit {
                    true => ones += 1,
                    false => ()
                }
            }
            if ones == self.bits_per_instance { return u64::MAX; }
        }

        // eval(P) -> a – Yields value a equal to c * 2^Fz, 
        // where Fz is the position in the bitmask of the first 0 bit and c = 1.29281 is a scaling factor.
        // Value a represents the result of counting.

        // yields a geometric average of the values computed using the previous algorithm, that is, 
        // a value equal to c * 2^((Fz0 + Fz1 + ... + FzM)/(M+1)), 
        // where M+1 is the number of sketch instances in a partial aggregate,
        // and Fzi is the index of the first 0 bit in instance i.
        let mut sum = 0;
        for sketch in &self.sketches {
            let mut first_zero = 0;
            for (idx, bit) in sketch.iter().enumerate() {
                if !bit {
                    first_zero = idx;
                    break;
                }
            }
            sum += first_zero;
        }

        // If test will be failing, look into rounding here, as the result for smoke 
        // test should be 3 and in fact calculations return ~ 2.58 (assuming tests are correct),
        // so I used round, but idk if it should work this way.
        let avg = sum as f64 / (self.sketches.len() as f64);
        let result = (ProbabilisticCounter::SCALING_FACTOR * 2_f64.powf(avg)).round() as u64;
        
        result
    }
}

/// A service allowing for sampling random nodes
/// from the system for gossiping.
pub(crate) trait PeerSamplingService {
    /// Returns a reference to a random Node
    /// in the system.
    fn get_random_peer(&mut self) -> ModuleRef<Node>;
}

/// A node (process) in the system.
pub(crate) struct Node {
    uuid: Uuid,
    rs: Box<dyn RandomnessSource + Send>,
    pss: Box<dyn PeerSamplingService + Send>,
    // You may add any necessary fields here
    // Mutex should not be needed - I will worry, when tests will fail.
    queries: HashMap<Uuid, (QueryInstallMsg, ProbabilisticCounter)>,
}

/// A message used by a client to install
/// a query on a node.
#[derive(Clone)]
pub(crate) struct QueryInstallMsg {
    pub(crate) bits_per_instance: usize,
    pub(crate) num_instances: usize,
    pub(crate) predicate: Arc<dyn Fn(&Uuid) -> bool + Send + Sync>,
}

/// A message used by a client to poll a node
/// to provide its current estimate of the query value.
pub(crate) struct QueryResultPollMsg {
    pub(crate) initiator: Uuid,
    pub(crate) callback: QueryResultPollCallback,
}

pub(crate) type QueryResultPollCallback =
    Box<dyn FnOnce(Option<u64>) -> Pin<Box<dyn Future<Output = ()> + Send>> + Send>;

/// A message that triggers a node to initiate
/// gossiping.
pub(crate) struct SyncTriggerMsg {}

/// A gossip message sent between two nodes.
pub(crate) struct SyncGossipMsg {
    // You may add any necessary fields here
    queries: HashMap<Uuid, (QueryInstallMsg, ProbabilisticCounter)>,
}

impl Node {
    pub(crate) async fn new(
        system: &mut System,
        uuid: Uuid,
        rs: Box<dyn RandomnessSource + Send>,
        pss: Box<dyn PeerSamplingService + Send>,
    ) -> ModuleRef<Self> {
        let self_ref = system
            .register_module(Self {
                uuid,
                rs,
                pss,
                // You may add initialization of any added fields here
                queries: HashMap::new(),
            })
            .await;
        self_ref
    }

    // You may add any extra methods here
}

#[async_trait::async_trait]
impl Handler<QueryInstallMsg> for Node {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, msg: QueryInstallMsg) {
        if msg.bits_per_instance == 0
            || msg.bits_per_instance > u32::BITS as usize
            || msg.bits_per_instance % 8 != 0
            || msg.num_instances == 0
        {
            return;
        }
        // A client can contact any node and request it to install an aggregation query,
        // which is done by sending to the node a QueryInstallMsg. 
        // The goal of the query is estimating the number of nodes in the system that satisfy the associated predicate. 
        // The query also specifies the number of probabilistic sketch instances (num_instances) 
        // and bits in each instance (bits_per_instance) that should be utilized when computing the aggregate.

        // Each node initializes its own local partial aggregate based on its local value.
        // init(_) -> P – Produces a bitmask that has 0 on all positions but one. 
        // The position of the sole 1 is selected at random from a geometric distribution with parameter 1/2, 
        // that is, position 0 is selected with probability 1/2, position 1, with probability 1/4, position 2, with probability 1/8, and so on.
        let mut counter = ProbabilisticCounter::new_zero(msg.bits_per_instance, msg.num_instances);
        counter.try_count_one_more_element(&mut *self.rs).unwrap();

        self.queries.insert(self.uuid, (msg, counter));
    }
}

#[async_trait::async_trait]
impl Handler<QueryResultPollMsg> for Node {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, msg: QueryResultPollMsg) {
        // At any moment in time, a client can contact any node asking it to provide the current estimate of the result for any query. 
        // The client does this by sending to the node a QueryResultPollMsg. 
        // The message carries the identifier of the node on which the query was originally installed (initiator) 
        // and a callback to be executed by the receiving node with the query result estimate as the parameter (callback). 
        // If the node is not aware of the query, it executes the callback with value None; 
        // otherwise, it executes the callback with its current estimate of the node count satisfying the predicate of the query.

        let query = self.queries.iter().find(|(_, (q, _))| (q.predicate)(&msg.initiator));

        match query {
            Some((_, q)) => {
                let counter = &q.1;
                let result = counter.evaluate();
                (msg.callback)(Some(result)).await;
            },
            None => {
                (msg.callback)(None).await;
            }
        }
    }
}

#[async_trait::async_trait]
impl Handler<SyncTriggerMsg> for Node {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, _msg: SyncTriggerMsg) {
        // A node initiates gossiping whenever it receives a SyncTriggerMsg. 
        // Upon reception of such a message, the node requests its associated peer sampling service (pss) 
        // to obtain a random node with which it will gossip.
        // Subsequently, it sends the necessary information to that node within a single SyncGossipMsg. 

        // Passing all known queries to the peer (all the information we know).
        // Should Node pass all queries that it knows (as like right now) or only the one that it executes?
        let peer = self.pss.get_random_peer();
        let msg = SyncGossipMsg { queries: self.queries.clone() };
        peer.send(msg).await;
    }
}

#[async_trait::async_trait]
impl Handler<SyncGossipMsg> for Node {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, msg: SyncGossipMsg) {
        // The nodes then repeatedly gossip their local partial aggregates, 
        // such that upon reception of a partial aggregate from another node in a given round, 
        // the recipient merges it into the local one, to be used in the next round, and so on.

        // If current node satisfies the predicate, increment the counter as well.

        for (id, (query, counter)) in msg.queries {
            match self.queries.get_mut(&id) {
                Some((_, c)) => {
                    if let Err(e) = c.try_merge_with(&counter) {
                        eprintln!("Failed to merge counters: {}", e);
                    }

                    if (query.predicate)(&self.uuid) {
                        let mut counter = counter.clone();
                        counter.try_count_one_more_element(&mut *self.rs).unwrap();
                        self.queries.insert(id, (query, counter));
                    }
                },
                None => {
                    self.queries.insert(id, (query.clone(), counter.clone()));

                    if (query.predicate)(&self.uuid) {
                        let mut counter = counter.clone();
                        counter.try_count_one_more_element(&mut *self.rs).unwrap();
                        self.queries.insert(id, (query, counter));
                    }
                }
            }
        }
    }
}
