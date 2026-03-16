package state

import (
	"crypto/ecdsa"
	"fmt"
	"log"
	"net"
	"time"
)

// RegisterOutgoingRequest registers an outgoing request in the state and returns the address of the target peer.
// Blocks until the request is registered or a timeout occurs.
func RegisterOutgoingRequest(sChan EventChanSend, reqID uint32, targetPeerID string, reqType byte) (*net.UDPAddr, error) {
	retChan := make(chan *outgoingMsgResp, 1)

	sChan <- outgoingMsg{
		details: ReqInfo{ReqID: reqID, TargetPeerID: targetPeerID, ReqType: reqType},
		retChan: retChan,
	}
	select {
	case resp := <-retChan:
		if resp.err != nil {
			return nil, fmt.Errorf("failed to register outgoing request %d to peer %s: %v", reqID, targetPeerID, resp.err)
		}
		return resp.addr, nil
	case <-time.After(timeoutForStateOperation):
		log.Printf("Timeout when registering request and trying to retrieve address from state for request %d", reqID)
		return nil, fmt.Errorf("timeout when registering request %d to peer %s", reqID, targetPeerID)
	}
}

// RegisterNewPeer registers a new peer in the state with its ID, address, and public key.
func RegisterNewPeer(sChan EventChanSend, peerID string, address *net.UDPAddr, publicKey *ecdsa.PublicKey) {
	sChan <- registerNewPeer{id: peerID, address: address, publicKey: publicKey}
}

// ValidateHelloReplyAndRegisterNewPeer validates a HelloReply message and registers a new peer if the reply is valid.
// Ignores the event if the reply does not match any sent request or if the request timed out.
// Blocks until the state checks the validity of the reply or a timeout occurs.
func ValidateHelloReplyAndRegisterNewPeer(sChan EventChanSend, msgID uint32, addr *net.UDPAddr, senderID string, senderPubKey *ecdsa.PublicKey) {
	retChan := make(chan *incomingMsgResp, 1) // buffered channel to avoid blocking the state
	sChan <- incomingHelloReplyMsg{
		reqID:   msgID,
		retChan: retChan,
	}
	select {
	case sResp := <-retChan:
		if sResp.Err != nil {
			// ignore the reply, as it didn't match any sent request or sent request timed out
			log.Printf("HelloReply from %s with ID: %d is not valid: %v", senderID, msgID, sResp.Err)
			return
		}
		log.Printf("HelloReply from %s with ID: %d is valid", senderID, msgID)
		RegisterNewPeer(sChan, senderID, addr, senderPubKey)
	case <-time.After(timeoutForStateOperation):
		log.Printf("Timeout waiting for state validity check for HelloReply from %s with ID: %d", senderID, msgID)
	}
}

func RegisterRootHash(sChan EventChanSend, addr *net.UDPAddr, rootHash []byte) {
	sChan <- registerRootHash{address: addr, rootHash: string(rootHash)}
}

// CreateIncomingMessageEvent creates an incoming message event in the state which checks if incoming message
// is from active peer, if incoming message is a reply, checks if the reply has corresponding request in sentRequests.
// If an active peer is found, returns it's public key, in order to verify the signature of the message if it is required.
// Blocks until the state checks the validity of the message or a timeout occurs.
func CreateIncomingMessageEvent(sChan EventChanSend, reqID uint32, reqType byte, isReply bool, senderAddress *net.UDPAddr) (*ecdsa.PublicKey, error) {
	retChan := make(chan *incomingMsgResp, 1) // buffered channel to avoid blocking the state
	sChan <- incomingMsg{
		reqID:         reqID,
		reqType:       reqType,
		isReply:       isReply,
		senderAddress: senderAddress,
		retChan:       retChan,
	}
	select {
	case sResp := <-retChan:
		if sResp.Err != nil {
			log.Printf("Incoming message from %s with ID: %d failed validity check: %v", senderAddress, reqID, sResp.Err)
			return nil, fmt.Errorf("incoming message from %s with ID: %d failed validity check: %v", senderAddress, reqID, sResp.Err)
		}
		log.Printf("Incoming message from %s with ID: %d is valid", senderAddress, reqID)
		return sResp.PubKey, nil
	case <-time.After(timeoutForStateOperation):
		log.Printf("Timeout waiting for state validity check for incoming message from %s with ID: %d", senderAddress, reqID)
		return nil, fmt.Errorf("timeout waiting for state validity check for incoming message from %s with ID: %d", senderAddress, reqID)
	}
}
