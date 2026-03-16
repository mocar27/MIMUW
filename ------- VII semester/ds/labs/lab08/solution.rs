use std::collections::{HashMap, HashSet};
use std::net::SocketAddr;
use std::sync::Arc;
use std::time::Duration;

use log::debug;
use serde::{Deserialize, Serialize};
use tokio::net::UdpSocket;
use uuid::Uuid;

use module_system::{Handler, ModuleRef, System, TimerHandle};

/// A message, which disables a process. Used for testing.
pub struct Disable;

/// A message, which enables a process. Used for testing.
pub struct Enable;

struct Init;

#[derive(Clone)]
struct Timeout;

pub struct FailureDetectorModule {
    enabled: bool,
    timeout_handle: Option<TimerHandle>,
    delta: Duration,
    delay: Duration,
    // add whatever fields necessary.
    alive: HashSet<Uuid>,
    suspected: HashSet<Uuid>,
    all_adresses: HashMap<Uuid, SocketAddr>,
    self_id: Uuid,
    socket: Arc<UdpSocket>,
}

impl FailureDetectorModule {
    pub async fn new(
        system: &mut System,
        delta: Duration,
        addresses: &HashMap<Uuid, SocketAddr>,
        ident: Uuid,
    ) -> ModuleRef<Self> {
        let addr = addresses.get(&ident).unwrap();
        let socket = Arc::new(UdpSocket::bind(addr).await.unwrap());

        let module_ref = system
            .register_module(Self {
                enabled: true,
                timeout_handle: None,
                delta,
                delay: delta,
                // initialize the fields you added
                // Initially (and also as stated in the lecture's algorithm) the process is alive,
                // at the beginning and when you can send a message to the process (so basically as well - at the beginning).
                alive: addresses.keys().cloned().collect(),
                suspected: HashSet::new(),
                all_adresses: addresses.clone(),
                self_id: ident.clone(),
                socket: socket.clone(),
            })
            .await;

        tokio::spawn(deserialize_and_forward(socket, module_ref.clone()));

        module_ref.send(Init).await;

        module_ref
    }
}

#[async_trait::async_trait]
impl Handler<Init> for FailureDetectorModule {
    async fn handle(&mut self, self_ref: &ModuleRef<Self>, _msg: Init) {
        self.timeout_handle = Some(self_ref.request_tick(Timeout, self.delay).await);
    }
}

/// New operation arrived at a socket.
#[async_trait::async_trait]
impl Handler<DetectorOperationUdp> for FailureDetectorModule {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, item: DetectorOperationUdp) {
        if self.enabled {
            let detector = item.0;
            let sock_addr = item.1;

            match detector {
                DetectorOperation::HeartbeatRequest => {
                    // Sending a request over the socket used for communication with processes
                    // with my id, so processes know which process to reply to.
                    let response = DetectorOperation::HeartbeatResponse(self.self_id);
                    let serialized = bincode::serialize(&response).unwrap();
                    self.socket.send_to(&serialized, sock_addr).await.unwrap();
                }
                DetectorOperation::HeartbeatResponse(uuid) => {
                    // We've received the reponse to heartbeat - process with uuid is alive, mark it.
                    self.alive.insert(uuid);
                }
                DetectorOperation::AliveRequest => {
                    // Sending a request over the socket used for communication with processes
                    // with requested data.
                    let response = DetectorOperation::AliveInfo(self.alive.clone());
                    let serialized = bincode::serialize(&response).unwrap();
                    self.socket.send_to(&serialized, sock_addr).await.unwrap();
                }
                DetectorOperation::AliveInfo(_alive) => {}
                // placeholder
            }
        }
    }
}

/// Called periodically to check send broadcast and update alive processes.
#[async_trait::async_trait]
impl Handler<Timeout> for FailureDetectorModule {
    async fn handle(&mut self, self_ref: &ModuleRef<Self>, _msg: Timeout) {
        if self.enabled {
            // what the process is doing on every timeout interval
            if !self.alive.is_disjoint(&self.suspected) {
                self.delay += self.delta;
            }

            for p in self.all_adresses.keys() {
                if !self.alive.contains(p) && !self.suspected.contains(p) {
                    self.suspected.insert(p.clone());
                }
                else if self.alive.contains(p) && self.suspected.contains(p) {
                    self.suspected.remove(p);
                }
            }

            let request = DetectorOperation::HeartbeatRequest;
            let serialized = bincode::serialize(&request).unwrap();
            for (_, addr) in self.all_adresses.iter() {
                self.socket.send_to(&serialized, addr).await.unwrap();
            }

            self.alive.clear();
            self.timeout_handle = Some(self_ref.request_tick(Timeout, self.delay).await);   
        }
    }
}

#[async_trait::async_trait]
impl Handler<Disable> for FailureDetectorModule {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, _msg: Disable) {
        self.enabled = false;
    }
}

#[async_trait::async_trait]
impl Handler<Enable> for FailureDetectorModule {
    async fn handle(&mut self, _self_ref: &ModuleRef<Self>, _msg: Enable) {
        self.enabled = true;
    }
}

async fn deserialize_and_forward(
    socket: Arc<UdpSocket>,
    module_ref: ModuleRef<FailureDetectorModule>,
) {
    let mut buffer = vec![0];
    while let Ok((len, sender)) = socket.peek_from(&mut buffer).await {
        if len == buffer.len() {
            buffer.resize(2 * buffer.len(), 0);
        } else {
            socket.recv_from(&mut buffer).await.unwrap();
            match bincode::deserialize(&buffer) {
                Ok(msg) => module_ref.send(DetectorOperationUdp(msg, sender)).await,
                Err(err) => {
                    debug!("Invalid format of detector operation ({})!", err);
                }
            }
        }
    }
}

struct DetectorOperationUdp(DetectorOperation, SocketAddr);

#[derive(Serialize, Deserialize)]
pub enum DetectorOperation {
    /// Request to receive a heartbeat.
    HeartbeatRequest,
    /// Response to heartbeat, contains uuid of the receiver of HeartbeatRequest.
    HeartbeatResponse(Uuid),
    /// Request to receive information about working processes.
    AliveRequest,
    /// Vector of processes which are alive according to AliveRequest receiver.
    AliveInfo(HashSet<Uuid>),
}
