# Advanced Networks project
Advanced network programming course final project which is implementation of hybrid protocol. The goal of the project is to implement a distributed P2P read-only file system.

# Introduction
The goal of this project is to implement a distributed read-only file system: every peer exports a
filesystem tree that is made available to all other peers. The tree exported by a peer may change
at any time, but a peer cannot modify the files exported by a different peer.
For cryptography, signing and verifying messages we use ECDSA (Elliptic Curve Digital Signature Algorithm)
type of crypto.

# Building the project
Run the bash script `build.sh` to build it.
All of built binaries will be placed in the `bin` directory to run freely.
The building script will create executable binary file `peer` to be run.
`peer` binary can be provided with the following arguments:
```
peer [-name <peer_name>] [-n <number_of_addresses>] [-debug]
```
which are in default set to (in order): sample-peer-name, 1, false.

Run the bash script `cleanup.sh` to clean all files created during compilation and/or execution.

# Protocol preliminary section
The protocol is a hybrid protocol:
- Central REST server serves as a rendez-vous point and as a channel to distribute cryptographic keys;
- Data transfer happens directly between peers, over UDP. 
- Every peer is identified by a name, which is an arbitrary string. 
- Peer names are unique: the server rejects duplicate registrations.
- The protocol uses cryptographic techniques in three places:
  - communication with the central server happens over HTTP protected by TLS (HTTPS);
  - data stored on peers are represented as a Merkle tree;
  - messages exchanged between peers are signed with cryptographic signatures.

### Peer discovery
A peer discovers other peers by contacting the server over a REST-like API. The
server maintains one or more socket addresses for every peer, as well as a cryptographic public key.

### Registration with the server 
Registration with the server happens in two steps: first, the client 
sends its cryptographic signature to the server using a PUT request over the HTTP API. It then
registers each of its IP addresses by sending a _Hello_ request to the server.
After the client sends a _Hello_ request to the server, the server will verify that the client is able
to receive requests by sending a _Hello_ request to the client. If the client doesn’t reply to the _Hello_
request with a properly signed message, its address will not be published by the server.

### Handshake 
In order to communicate, two peers exchange _Hello_ and _HelloReply_ messages. These
messages are protected by cryptographic signatures. 

### Data transfer 
Every peer maintains a content-indexed database of pieces of data: values are
arbitrary pieces of data, while keys are the SHA-256 hashes of the data. A peer requests pieces of
data by sending _DatumRequest_ messages. Since data are protected by end-to-end hashes in the form of a Merkle tree, 
_Datum_ messages do not need to be protected by a cryptographic signature.

## Project plan 
- [x] Client-Server all-in communication (Chapter 3. of the task description)
- [x] Register with the server and maintain its association for unbounded periods of time.
- [ ] Make files available to other peers, both when behind NAT and not behind NAT.
- [ ] Make directories of less than 16 entries available to other peers.
- [ ] Useful user interface as extension.
- [ ] Download files from a peer not behind NAT.
- [ ] Implementation being able to download single files selected by the user.

## Considerations
- Registration with the server and maintenance of its association for unbounded periods of time works
  (take into consideration, that server automatically expire peers after 30 minutes (relating to task description,
  it cannot really be dealt with).
- When communicating, we create a message of bytes we want to pass to some other Peer and send it using WriteToUDP,
so I guess, we are sending a single packet in flight, when communicating.
- **Not implementing** extensions mechanism (Chapter 4.2 of being intermediate node 
in the NAT Traversal due to limited time resources)
- **Not implementing** extensions mentioned in the Chapter 7. of the task (except UI, as of limited time resources).
 