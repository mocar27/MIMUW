use std::io::{Read, Write};
use std::sync::Arc;

use rustls::pki_types::ServerName;
use rustls::pki_types::pem::PemObject;
use rustls::pki_types::{CertificateDer, PrivateKeyDer};
use rustls::{ClientConfig, ClientConnection, RootCertStore, ServerConfig, ServerConnection, StreamOwned};

use hmac::{Hmac, Mac};
use sha2::Sha256;

type HmacSha256 = Hmac<Sha256>;

// You can add here other imports from std or crates listed in Cargo.toml.

pub struct SecureClient<L: Read + Write> {
    client_connection: StreamOwned<ClientConnection, L>,
    // As HMAC key doesn't have to have 32 bytes, we need to store it in a Vec<u8>
    hmac_key: Vec<u8>,
}

pub struct SecureServer<L: Read + Write> {
    server_connection: StreamOwned<ServerConnection, L>,
    // As HMAC key doesn't have to have 32 bytes, we need to store it in a Vec<u8>
    hmac_key: Vec<u8>,
}

impl<L: Read + Write> SecureClient<L> {
    /// Creates a new instance of SecureClient.
    ///
    /// SecureClient communicates with SecureServer via `link`.
    /// The messages include a HMAC tag calculated using `hmac_key`.
    /// A certificate of SecureServer is signed by `root_cert`.
    /// We are connecting with `server_hostname`.
    pub fn new(
        link: L,
        hmac_key: &[u8],
        root_cert: &str,
        server_hostname: ServerName<'static>,
    ) -> Self {

        // Create Client connection as in examples/rustls/main.rs file
        let mut root_store = RootCertStore::empty();
        root_store.add_parsable_certificates(CertificateDer::from_pem_slice(
            root_cert.as_bytes(),
        ));

        let client_config = ClientConfig::builder()
            .with_root_certificates(root_store)
            .with_no_client_auth();

        let connection = ClientConnection::new(Arc::new(client_config), server_hostname).unwrap();

        SecureClient {
            client_connection: StreamOwned::new(connection, link),
            hmac_key: hmac_key.to_vec(),
        }
    }

    /// Sends the data to the server. The sent message follows the
    /// format specified in the description of the assignment.
    pub fn send_msg(&mut self, data: Vec<u8>) {
        
        // Calculate HMAC tag as in examples/hmac.rs file
        let mut mac = HmacSha256::new_from_slice(&self.hmac_key).unwrap();
        mac.update(&data);
        let tag = mac.finalize().into_bytes();

        // Create a message to be sent
        let mut msg = Vec::new();
        msg.extend_from_slice(&(data.len() as u32).to_be_bytes());
        msg.extend_from_slice(&data);
        msg.extend_from_slice(&tag);

        // Actually send the message
        self.client_connection.write_all(&msg).unwrap();
    }
}

impl<L: Read + Write> SecureServer<L> {
    /// Creates a new instance of SecureServer.
    ///
    /// SecureServer receives messages from SecureClients via `link`.
    /// HMAC tags of the messages are verified against `hmac_key`.
    /// The private key of the SecureServer's certificate is `server_private_key`,
    /// and the full certificate chain is `server_full_chain`.
    pub fn new(
        link: L,
        hmac_key: &[u8],
        server_private_key: &str,
        server_full_chain: &str,
    ) -> Self {

        // Create Server connection as in examples/rustls/main.rs file
        let certs: Vec<CertificateDer<'static>> = CertificateDer::pem_slice_iter(server_full_chain.as_bytes())
            .flatten()
            .collect();

        let private_key = PrivateKeyDer::from_pem_slice(server_private_key.as_bytes())
            .unwrap();

        let server_config = ServerConfig::builder()
            .with_no_client_auth()
            .with_single_cert(certs, private_key)
            .unwrap();

        let connection = ServerConnection::new(Arc::new(server_config)).unwrap();

        SecureServer {
            server_connection: StreamOwned::new(connection, link),
            hmac_key: hmac_key.to_vec(),
        }
    }

    /// Receives the next incoming message and returns the message's content
    /// (i.e., without the message size and without the HMAC tag) if the
    /// message's HMAC tag is correct. Otherwise, returns `SecureServerError`.
    pub fn recv_message(&mut self) -> Result<Vec<u8>, SecureServerError> {
        // Read the message size
        let mut buf = [0u8; 4];
        self.server_connection.read_exact(&mut buf).unwrap();
        let msg_size = u32::from_be_bytes(buf) as usize;

        // Read the message
        let mut buf = vec![0u8; msg_size];
        self.server_connection.read_exact(&mut buf).unwrap();
        let msg = buf;

        // Read the HMAC tag
        let mut buf = [0u8; 32];
        self.server_connection.read_exact(&mut buf).unwrap();
        let tag = buf;

        // Verify the HMAC tag as in examples/hmac.rs file
        let mut mac = HmacSha256::new_from_slice(&self.hmac_key).unwrap();
        mac.update(&msg);

        if mac.verify_slice(&tag).is_err() {
            return Err(SecureServerError::InvalidHmac);
        }

        Ok(msg)
    }
}

#[derive(Copy, Clone, Eq, PartialEq, Hash, Debug)]
pub enum SecureServerError {
    /// The HMAC tag of a message is invalid.
    InvalidHmac,
}

// You can add any private types, structs, consts, functions, methods, etc., you need.
