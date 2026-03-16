package state

import (
	"fmt"
	"log"

	communicationCommon "github.com/Toomimi/advanced-networks/communication/common"
	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
)

func handleRegisterNewPeer(s *state, m registerNewPeer) {
	log.Println("State attempts to register new peer: ", m)
	s.registerNewPeer(m)
	s.ifaceSender <- NewPeerIFace{PeerID: m.id} // inform the iface about the new peer
}

func handleIncomingHelloReplyMsg(s *state, m incomingHelloReplyMsg) {
	log.Println("State recorded new HelloReply with id: ", m.reqID)
	if !s.checkIfValidReply(m.reqID, communicationCommon.HelloReply) {
		m.retChan <- &incomingMsgResp{nil, fmt.Errorf("no request found for the reply with id: %d", m.reqID)}
		return
	}
	m.retChan <- &incomingMsgResp{nil, nil} // err field is nil, meaning that the reply matches the sent request
}

func handleRegisterRootHash(s *state, m registerRootHash) {
	peerID, err := s.getPeerID(m.address)
	if err != nil {
		log.Printf("Failed to register rootHash  for address %s: %v", m.address, err)
		return
	}
	s.registerRootHash(peerID, m.rootHash)
	s.ifaceSender <- PeerRootHash{PeerID: peerID, RootHash: m.rootHash}
}

func handleOutgoingMsgEvent(s *state, m outgoingMsg) {
	log.Printf("State recorded new request to %s with type %d at %s", m.details.TargetPeerID, m.details.ReqType, m.details.sentAt)

	if m.details.ReqType != communicationCommon.Hello && !s.checkIfPeerActiveAndRefreshPingTimer(m.details.TargetPeerID) {
		m.retChan <- &outgoingMsgResp{nil, fmt.Errorf("peer %s is not active", m.details.TargetPeerID)}
		return
	}

	err := s.addNewRequest(m.details)
	if err != nil {
		m.retChan <- &outgoingMsgResp{nil, err}
		return
	}

	if m.details.ReqType == communicationCommon.Hello {
		m.retChan <- &outgoingMsgResp{nil, nil}
		return
	}

	addr, err := s.getTargetAddress(m.details.TargetPeerID)
	m.retChan <- &outgoingMsgResp{addr, err}
}

func handleIncomingMsgEvent(s *state, m incomingMsg) {
	log.Printf("State recorded incoming message with id: %d", m.reqID)
	peerID, err := s.getPeerID(m.senderAddress)
	if err != nil {
		m.retChan <- &incomingMsgResp{nil, err}
		return
	}

	if !s.checkIfPeerActiveAndRefreshTimers(peerID) {
		log.Printf("Ping and expiry timer refresh failed for peer %s", peerID)
		m.retChan <- &incomingMsgResp{nil, fmt.Errorf("peer %s is not active", peerID)}
		return
	}

	if !m.isReply {
		m.retChan <- &incomingMsgResp{
			PubKey: nil, // we don't need to return public key for non-reply messages
			Err:    nil, // no error, meaning that the message is valid and was processed successfully
		}
		return
	}

	// at this stage we parse incoming reply to active peer
	if !s.checkIfValidReply(m.reqID, m.reqType) {
		m.retChan <- &incomingMsgResp{nil, fmt.Errorf("missing id for the reply from peer: %s", peerID)}
		return
	}

	// pubkey is not required for every reply / request, but we can return it to handler for all messages
	pubKey, err := s.getPubKey(peerID)
	if err != nil {
		m.retChan <- &incomingMsgResp{nil, err}
		return
	}

	m.retChan <- &incomingMsgResp{
		PubKey: pubKey,
		Err:    nil,
	}
}

// TODO - For Datum request receiving, verify that the hashes of Request and Reply are equal.
// And further hash the data and verify whether hashed value matches the hash in the reply.
// Store it if needed.

func HandleState(eventChan EventChan, ifaceSender ifaceChanSend, msgDispatcherSender msgDisp.EventChanSend) {
	log.Println("Starting state handler")
	s := newState(eventChan, ifaceSender, msgDispatcherSender)
	for {
		select {
		case msg := <-eventChan:
			switch m := msg.(type) {
			case registerNewPeer:
				handleRegisterNewPeer(s, m)
			case incomingHelloReplyMsg:
				handleIncomingHelloReplyMsg(s, m)
			case registerRootHash:
				handleRegisterRootHash(s, m)
			case outgoingMsg:
				handleOutgoingMsgEvent(s, m)
			case incomingMsg:
				handleIncomingMsgEvent(s, m)
			default:
				log.Fatalf("State received unknown event type: %T", msg)
			}
		case peerID := <-s.expiredPeerChan:
			log.Printf("State handler received peer's id %s to expire", peerID)
			s.removePeer(peerID)
			log.Printf("State handler removed peer %s from state", peerID)
			s.ifaceSender <- PeerExpiredIFace{PeerID: peerID}
		}
	}
}
