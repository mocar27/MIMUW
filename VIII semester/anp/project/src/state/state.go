package state

import (
	"crypto/ecdsa"
	"fmt"
	"log"
	"net"
	"time"

	communicationCommon "github.com/Toomimi/advanced-networks/communication/common"
	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
)

const (
	requestTimeout           = 15 * time.Second // timeout for requests
	peerTimeout              = 5 * time.Minute  // timeout for peers to be considered active
	pingTimeout              = 90 * time.Second // timeout for pings, after which the peer is considered inactive and its expiry timer is reset
	timeoutForStateOperation = 2 * time.Second  // timeout for state operations, such as adding a new request or checking if a peer is active
)

type peerInfo struct {
	id          string
	address     *net.UDPAddr // we assume the first address is the primary one
	publicKey   *ecdsa.PublicKey
	pingTimer   *time.Timer
	expiryTimer *time.Timer // timer to check if the peer is still active
	rootHash    string
}

type ReqInfo struct {
	ReqID        uint32
	TargetPeerID string
	ReqType      byte
	sentAt       time.Time // filled when the request is recorded by state
}

type state struct {
	stateSender         EventChanSend // sender channel for messages to the state
	ifaceSender         ifaceChanSend
	msgDispatcherSender msgDisp.EventChanSend // channel to send messages to the message dispatcher
	expiredPeerChan     chan string           // channel to send expired peer IDs to the HandleState goroutine
	activePeers         map[string]*peerInfo
	sentRequests        map[uint32]*ReqInfo
	addressToPeerID     map[string]string // map to store addresses to peer IDs, so we can find peer by address
}

func newState(stateSender EventChanSend, ifaceSender ifaceChanSend, msgDispatcherSender msgDisp.EventChanSend) *state {
	expirePeerChan := make(chan string, 100) // buffered channel to send expired peer IDs to the HandleState goroutine

	return &state{
		stateSender:         stateSender,
		ifaceSender:         ifaceSender,
		msgDispatcherSender: msgDispatcherSender,
		expiredPeerChan:     expirePeerChan,
		activePeers:         make(map[string]*peerInfo),
		sentRequests:        make(map[uint32]*ReqInfo),
		addressToPeerID:     make(map[string]string),
	}
}

// expirePeer is called when a peer is considered inactive after the expiry timer expires.
// It sends a message to the HandleState goroutine to remove the peer from activePeers so the state
// can be updated and the user iface informed about the change.
func expirePeer(peerID string, expiredPeerChan chan<- string) {
	log.Println("Scheduling expiry of peer with id ", peerID)
	expiredPeerChan <- peerID // send the peer ID to the channel to be handled by HandleState
}

// pingPeer builds a ping message and creates an event for message dispatcher that
// dispatches it to peer that has to be pinged.
func pingPeer(sChan EventChanSend, msgDispSender msgDisp.EventChanSend, peerID string) {
	log.Println("Scheduling ping for peer with id ", peerID)
	reqID := communicationCommon.GenerateRandomID()
	pingReq := communicationCommon.BuildPingMessage(reqID, communicationCommon.Ping)

	// ping timer is reset in checkIfPeerActiveAndRefreshPingTimer during request registration,
	// so we don't have to reset it here
	addr, err := RegisterOutgoingRequest(sChan, reqID, peerID, communicationCommon.Ping)
	if err != nil {
		log.Printf("Skipping ping for peer: %s: %v", peerID, err)
		return
	}
	fmt.Printf("Sending ping to peer %s with address %s\n", peerID, addr)
	msgDisp.CreateDispatchMessageEvent(msgDispSender, pingReq, addr, false)
}

func (s *state) registerAddressForPeer(peerID string, addr *net.UDPAddr) {
	log.Printf("Registering in state address %s for peer %s", addr, peerID)
	// we assume that the address is unique for each peer, so we don't worry about overwrite
	s.addressToPeerID[addr.String()] = peerID
}

func (s *state) registerNewPeer(peer registerNewPeer) {
	if _, exists := s.activePeers[peer.id]; exists {
		log.Printf("Warning: State tried adding peer with id %s that already exist", peer.id)
		// peer already registered, but it can add new address
		s.registerAddressForPeer(peer.id, peer.address)
	} else {
		s.activePeers[peer.id] = &peerInfo{
			id:          peer.id,
			address:     peer.address, // we assume the first address is the primary one
			publicKey:   peer.publicKey,
			rootHash:    "", // root hash will be set later when the peer sends it
			pingTimer:   time.AfterFunc(pingTimeout, func() { pingPeer(s.stateSender, s.msgDispatcherSender, peer.id) }),
			expiryTimer: time.AfterFunc(peerTimeout, func() { expirePeer(peer.id, s.expiredPeerChan) }),
		}
		s.registerAddressForPeer(peer.id, peer.address)
		log.Printf("Registered new peer %s in state", peer.id)
	}
}

// addNewRequest adds a new request to the sentRequests map, so it can be tracked if
// replies match the ID or timed out.
func (s *state) addNewRequest(reqDetails ReqInfo) error {
	if otherReq, exists := s.sentRequests[reqDetails.ReqID]; exists {
		if otherReq.sentAt.Add(requestTimeout).After(time.Now()) {
			log.Printf("Warning: state tried adding request with id %d that already exists", reqDetails.ReqID)
			return fmt.Errorf("request with id %d already exists", reqDetails.ReqID)
		}
		// expired request with the same id, it can be overwritten
	}
	reqDetails.sentAt = time.Now()
	copied := reqDetails
	s.sentRequests[reqDetails.ReqID] = &copied
	return nil
}

func (s *state) getTargetAddress(peerID string) (*net.UDPAddr, error) {
	if peer, exists := s.activePeers[peerID]; exists {
		return peer.address, nil
	}
	log.Printf("Retrieving address for peer %s failed. Peer not found in active peers", peerID)
	return nil, fmt.Errorf("peer %s not found in active peers", peerID)
}

// checkIfPeerActiveAndRefreshTimers checks if peer is active and refreshes its expiry and ping timers
// if it is. Returns false if the peer is not present in activePeers (removed itself after expiry)
// or if the expiry timer scheduled removal. Returns true if successful.
func (s *state) checkIfPeerActiveAndRefreshTimers(peerID string) bool {
	if peer, exists := s.activePeers[peerID]; exists {
		log.Printf("Attempting refresh of expiry and ping timer for peer %s", peerID)
		if !peer.expiryTimer.Stop() {
			// expiry timer scheduled removal or expired
			return false
		}
		peer.expiryTimer.Reset(peerTimeout)

		// we don't have to stop ping timer and check whether it scheduled a ping, because at most it will send an extra ping
		peer.pingTimer.Reset(pingTimeout)

		return true
	}
	return false
}

// checkIfPeerActiveAndRefreshPingTimer checks if peer is active and refreshes its ping
// timer if it is. Returns false if the peer is not present in activePeers and
// true if resets the timer.
func (s *state) checkIfPeerActiveAndRefreshPingTimer(peerID string) bool {
	log.Printf("Attempting refresh of ping timer for peer %s", peerID)
	if peer, exists := s.activePeers[peerID]; exists {
		peer.pingTimer.Reset(peerTimeout)
		return true
	}

	log.Printf("Ping timer refresh failed for peer %s, peer not found in active peers", peerID)
	return false
}

// getPeerID retrieves the peer ID for a given sender address.
func (s *state) getPeerID(senderAddress *net.UDPAddr) (string, error) {
	// check if the sender address is in the addressToPeerID map
	if peerID, exists := s.addressToPeerID[senderAddress.String()]; exists {
		// check if the peer is active and refresh its expiry timer
		return peerID, nil
	}
	log.Printf("Warning: Sender address %s not found in addressToPeerID map", senderAddress)
	return "", fmt.Errorf("sender address %s not found in addressToPeerID map", senderAddress)
}

// checkIfValidReply checks if the reply is valid for the request with the given ID. Checks if the
// request exists in sentRequests, if the reply type matches the request type, and if the request
// has not timed out. If reply valid or timed-out, request removed from sentRequests.
func (s *state) checkIfValidReply(reqID uint32, replyType byte) bool {
	sentReq, exists := s.sentRequests[reqID]
	if !exists {
		log.Printf("Received reply for request %d, but it does not exist in sentRequests", reqID)
		return false
	}
	if !communicationCommon.CheckIfRequestAndReply(sentReq.ReqType, replyType) {
		log.Printf("Received reply for request %d, but it has different type than the original request", reqID)
		return false
	}

	delete(s.sentRequests, reqID) // remove the request from sentRequests as it was replied to or expired
	if time.Now().After(sentReq.sentAt.Add(requestTimeout)) {
		log.Printf("Received reply for request %d, but it has timed out", reqID)
		return false // the request has timed out, so we consider the reply invalid
	}

	return true
}

// getPubKey retrieves the public key for a given peer ID from the activePeers map.
func (s *state) getPubKey(peerID string) (*ecdsa.PublicKey, error) {
	if peer, exists := s.activePeers[peerID]; exists {
		return peer.publicKey, nil
	}
	log.Printf("Warning: Peer %s not found in active peers", peerID)
	return nil, fmt.Errorf("couldn't obtain public key, peer %s not found in active peers", peerID)
}

// removePeer removes a peer from the activePeers map and stops its timers.
func (s *state) removePeer(peerID string) {
	log.Printf("Removing peer %s from active peers", peerID)

	if peer, exists := s.activePeers[peerID]; exists {
		// stop timers to prevent further actions on this peer
		if !peer.expiryTimer.Stop() {
			log.Printf("While removing peer %s its expiry timer was already stopped or expired", peerID)
		}
		if !peer.pingTimer.Stop() {
			log.Printf("While removing peer %s its ping timer was already stopped or expired", peerID)
		}

		delete(s.activePeers, peerID) // remove the peer from activePeers
		for addr := range s.addressToPeerID {
			if s.addressToPeerID[addr] == peerID {
				delete(s.addressToPeerID, addr) // remove the address mapping
			}
		}
	} else {
		log.Printf("Warning: Tried to remove non-existing peer %s", peerID)
	}
}

func (s *state) registerRootHash(peerID string, rootHash string) {
	if peer, exists := s.activePeers[peerID]; exists {
		peer.rootHash = rootHash
		log.Printf("State registered new root hash for peer %s: %s", peerID, peer.rootHash)
	} else {
		log.Printf("Warning: State tried to register root hash for non-existing peer %s", peerID)
	}
}
