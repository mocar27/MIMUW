use module_system::{Handler, ModuleRef, System};
use std::collections::{VecDeque, HashSet};

// Since queue crate for queing messages in not available in the given Cargo.toml
// we will use the VecDeque from the std::collections to store messages 
// in order to execute them properly.

// Eventual consistency -- at some point, when the users / processes stop
// writing / doing some operations that involve coordination, the system 
// will eventually show at the end the same state of the environment 
// for all the users (after successfully applying all the operations).

/// Marker trait indicating that a broadcast implementation provides
/// guarantees specified in the assignment description.
pub(crate) trait ReliableBroadcast<const N: usize> {}

#[async_trait::async_trait]
pub(crate) trait ReliableBroadcastRef<const N: usize>: Send + Sync + 'static {
    async fn send(&self, msg: Operation);
}

#[async_trait::async_trait]
impl<T, const N: usize> ReliableBroadcastRef<N> for ModuleRef<T>
where
    T: ReliableBroadcast<N> + Handler<Operation> + Send,
{
    async fn send(&self, msg: Operation) {
        self.send(msg).await;
    }
}

/// Marker trait indicating that a client implementation
/// follows specification from the assignment description.
pub(crate) trait EditorClient {}

#[async_trait::async_trait]
pub(crate) trait ClientRef: Send + Sync + 'static {
    async fn send(&self, msg: Edit);
}

#[async_trait::async_trait]
impl<T> ClientRef for ModuleRef<T>
where
    T: EditorClient + Handler<Edit> + Send,
{
    async fn send(&self, msg: Edit) {
        self.send(msg).await;
    }
}

/// Actions (edits) which can be applied to a text.
#[derive(Clone)]
#[cfg_attr(test, derive(PartialEq, Debug))]
pub(crate) enum Action {
    /// Insert the character at the position.
    Insert { idx: usize, ch: char },
    /// Delete a character at the position.
    Delete { idx: usize },
    /// A _do nothing_ operation. `Nop` cannot be issued by a client.
    /// `Nop` can only be issued by a process or result from a transformation.
    Nop,
}

impl Action {
    /// Apply the action to the text.
    pub(crate) fn apply_to(&self, text: &mut String) {
        match self {
            Action::Insert { idx, ch } => {
                text.insert(*idx, *ch);
            }
            Action::Delete { idx } => {
                text.remove(*idx);
            }
            Action::Nop => {
                // Do nothing.
            }
        }
    }
}

/// Client's request to edit the text.
#[derive(Clone)]
pub(crate) struct EditRequest {
    /// Total number of operations a client has applied to its text so far.
    pub(crate) num_applied: usize,
    /// Action (edit) to be applied to a text.
    pub(crate) action: Action,
}

/// Response to a client with action (edit) it should apply to its text.
#[derive(Clone)]
pub(crate) struct Edit {
    pub(crate) action: Action,
}

#[derive(Clone)]
pub(crate) struct Operation {
    /// Rank of a process which issued this operation.
    pub(crate) process_rank: usize,
    /// Action (edit) to be applied to a text.
    pub(crate) action: Action,
}

impl Operation {
    // Add any methods you need.
    // Operation transformation rules as defined in the task description.
    pub(crate) fn transform(&self, other: &Operation) -> Operation {
        let op1 = self.clone();
        let op2 = other.clone();

        let r1 = op1.process_rank;
        let r2 = op2.process_rank;
        
        let actn: Action = match (op1.action, op2.action) {
            (Action::Insert { idx: p1, ch: c1 }, Action::Insert { idx: p2, ch: _ }) => {
                if p1 < p2 { Action:: Insert { idx: p1, ch: c1 } }
                else if p1 == p2 && r1 < r2 { Action:: Insert { idx: p1, ch: c1 } }
                else { Action:: Insert { idx: p1 + 1, ch: c1 } }
            }
            (Action::Delete { idx: p1 }, Action::Delete { idx: p2 }) => {
                if p1 < p2 { Action:: Delete { idx: p1 } }
                else if p1 == p2 { Action:: Nop }
                else { Action:: Delete { idx: p1 - 1 } }
            }
            (Action::Insert { idx: p1, ch: c1 }, Action::Delete { idx: p2 }) => {
                if p1 <= p2 { Action:: Insert { idx: p1, ch: c1 } }
                else { Action:: Insert { idx: p1 - 1, ch: c1 } }
            }
            (Action::Delete { idx: p1 }, Action::Insert { idx: p2, ch: _ }) => {
                if p1 < p2 { Action:: Delete { idx: p1 } }
                else { Action:: Delete { idx: p1 + 1 } }
            }
            (action, _) => action,
        };

        Operation {
            process_rank: r1,
            action: actn,
        }
    }
}

/// Process of the system.
pub(crate) struct Process<const N: usize> {
    /// Rank of the process.
    rank: usize,
    /// Reference to the broadcast module.
    broadcast: Box<dyn ReliableBroadcastRef<N>>,
    /// Reference to the process's client.
    client: Box<dyn ClientRef>,
    // Add any fields you need.
    
    // To facilitate discovering which transformations have to be applied to an operation, 
    // each process stores locally a log of operations it has already applied to its text. 
    log: Vec<Operation>,

    // Client request to issue and edit operation, to be processed by me and broadcasted
    // to every other process in the system.
    // Cannot be of type Operation, as we need retrieve the "num_applied" and transform 
    // messages wrt. to that number.
    my_ereqs: VecDeque<EditRequest>,

    // If some older operations have not been already received and applied by the process, 
    // the process shall store the operation aside and apply it later when it is possible.
    next_round_ops: VecDeque<Operation>,

    // The round ends when the process receives operations from all other processes.
    processes: HashSet<usize>,

}

impl<const N: usize> Process<N> {
    pub(crate) async fn new(
        system: &mut System,
        rank: usize,
        broadcast: Box<dyn ReliableBroadcastRef<N>>,
        client: Box<dyn ClientRef>,
    ) -> ModuleRef<Self> {
        let self_ref = system
            .register_module(Self {
                rank,
                broadcast,
                client,
                // Add any fields you need.
                log: Vec::new(),
                next_round_ops: VecDeque::new(),
                processes: HashSet::new(),
                my_ereqs: VecDeque::new(),
            })
            .await;
        self_ref
    }

    // Add any methods you need.
    pub async fn start(&mut self, ereq: EditRequest) {
        // At the beginning of a new round, each process issues its operation 
        // (with respect to its current text, that is, the text resulting from the previous round),
        // appends the operation to the log, applies it to the text, and broadcasts to other processes.

        // Since the client communicates with its process in an asynchronous way, 
        // in each edit request, the client sends also the total number of operations it has applied to its text so far. 
        // This way, the process can learn with respect to which state the operation was issued,
        // and transforms the request with respect to operations which were appended to the log later.

        // These transformations shall follow the same transformation rules as concurrent operations, 
        // and the edit request shall be assigned a temporary process rank larger than ranks of actual processes 
        // (e.g., N+1 in a system containing N processes). After the transformations, the requested operation is appended to the log, 
        // broadcast, and sent back to the client (which applies it to the text only then).
        let mut op = Operation {
            process_rank: N + 1,
            action: ereq.action.clone(),
        };
        for log_op in &self.log[ereq.num_applied..] {
            op = op.transform(log_op);
        }
        
        op = Operation {
            process_rank: self.rank,
            action: op.action,
        };

        self.log.push(op.clone());
        self.processes.insert(self.rank);
        self.client.send(Edit { action: op.action.clone() }).await;
        self.broadcast.send(op).await;
    }
}

#[async_trait::async_trait]
impl<const N: usize> Handler<Operation> for Process<N> {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, msg: Operation) {
        // When the process receives an operation from the next round 
        // the operation is not processed until the next round.
        // (Process has already issued message in the current round),
        // so push and do not handle the operation.
        if self.processes.contains(&msg.process_rank) {
            self.next_round_ops.push_back(msg);
            return;
        }

        // If it is a reception of some other process’s operation, 
        // the process internally issues itself a NOP operation, 
        // handles it, and then continues processing the received operation. 
        if self.processes.is_empty() {
            let nop = EditRequest {
                num_applied: self.log.len(),
                action: Action::Nop,
            };
            self.start(nop).await;
        }

        // When a process receives a new operation issued by another process 
        // and it has already applied all operations older than this one, it examines its log (the least recent operation first) 
        // and transforms the received operation with respect to all operations from its log that are concurrent with the operation. 
        // Then, it appends the transformed operation to the log and applies it to the text.
        
        // 1. Examine & Transform
        let mut op = msg.clone();
        // Mark len of all operations in the log.
        let ops_len = self.log.len();
        // Mark number of responses in current round.
        let processes_len = self.processes.len();
        // Mark the start of the oprations examination.
        let start = ops_len.saturating_sub(processes_len); 

        for log_op in &self.log[start..] {
            op = op.transform(log_op);
        }

        // 2. Append to the log
        self.log.push(op.clone());

        // 3. Apply to the text (The process sends to its client an Edit message each time the client should apply a new operation to its text).
        self.client.send(Edit { action: op.action }).await;

        // 4. Mark the process as received
        self.processes.insert(msg.process_rank);

        if self.processes.len() == N {
            // The round ends when the process receives operations from all other processes.
            self.processes.clear();

            // As it's the end of the round, start the new one.
            // As described in the task, if we have some operation
            // start the round with this request, otherwise just start the round with Nop.
            if !self.my_ereqs.is_empty() {
                let my_ereq = self.my_ereqs.pop_front();
                if let Some(ereq) = my_ereq {
                    self.start(ereq).await;
                }
            }

            // Here in the if statement the round has ended as we have received operations from all other processes.
            // Process operations that were "intented" for the next round.
            // Operations within a round (as well next) are always processed in the order they are received.
            while !self.next_round_ops.is_empty() {
                let next_round_op = self.next_round_ops.pop_front();
                if let Some(next_op) = next_round_op {
                    self.handle(_self_ref, next_op).await;
                }
            }
        }
    }
}

#[async_trait::async_trait]
impl<const N: usize> Handler<EditRequest> for Process<N> {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, request: EditRequest) {
        // Received a request from client, but the process is not in the middle of a round.
        // Start it with given message (At the beginning of a new round, each process issues its operation).
        // If the event is a user’s request of a new edit, then the process continues with this operation.
        if self.processes.is_empty() {
            self.start(request).await;
        }
        else { 
            // If the process is currently processing the round, just add the edit request to queue.
            self.my_ereqs.push_back(request);
        }
    }
}

// This task description is so bad I can't even imagine it. I will never get what is the point of throwing different 
// types of specification "somewhere" across the task descripion. I mean there is a paragraph
// referencing to handling client message, then there is a paragraph referencing to handling processes message
// and then in each one of these, there is additional description (written with small font), that
// adds additional requirements (and changes the logic of handling) that are so easily to miss it's incredible.
