package communication

import (
	"encoding/binary"
	"log"
	"net"

	. "github.com/Toomimi/advanced-networks/communication/common"
	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
	"github.com/Toomimi/advanced-networks/crypto"
	"github.com/Toomimi/advanced-networks/state"
)

func ListenAndServe(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, conn *net.UDPConn, peerID string) {
	buf := make([]byte, 1400) // max expected datagram 1128? - unprecise
	for {
		n, addr, err := conn.ReadFromUDP(buf)
		if err != nil {
			log.Printf("Failed to read UDP: %v", err)
			continue
		}

		msgID := binary.BigEndian.Uint32(buf[:MsgTypeIndex])
		msgType := buf[MsgTypeIndex]
		log.Printf("Received %d bytes from %s where ID: %d, Type: %d", n, addr, msgID, msgType)

		switch msgType {
		case Ping:
			handlePingMessage(sChan, msgDispChan, addr, msgID)
		case Hello:
			handleHelloMessage(sChan, msgDispChan, addr, buf[:n], msgID, peerID)
		case RootRequest:
			handleRootRequest(sChan, msgDispChan, addr, msgID)
		case DatumRequest:
			handleDatumRequest(sChan, msgDispChan, addr, buf[:n], msgID)
		case NatTraversalRequest2:
			log.Printf("NatTraversalRequest2 not yet implemented")
		case Ok:
			handleOkMessage(sChan, addr, msgID)
		case Error:
			handleErrorMessage(addr, buf[:n])
		case HelloReply:
			handleHelloReply(sChan, addr, buf[:n], msgID)
		case RootReply:
			handleRootReply(sChan, addr, buf[:n], msgID)
		case Datum:
			handleDatumRequest(sChan, msgDispChan, addr, buf[:n], msgID)
		case NoDatum:
			handleNoDatumReply(sChan, addr, buf[:n], msgID)
		default:
			HandleUnknownMessage(msgDispChan, addr, msgID, msgType)
		}
	}
}

// checkValidityWithState checks if the message is from active peer and matches corresponding sent request if a reply.
// Ignores returned by the state publicKey of the sender, thus should be called only with *unsigned* messages.
func checkValidityWithState(sChan state.EventChanSend, msgID uint32, msgType byte, isReply bool, addr *net.UDPAddr) {
	_, err := state.CreateIncomingMessageEvent(sChan, msgID, msgType, isReply, addr)
	if err != nil {
		log.Printf("Failed to validate message with ID: %d from %s: %v", msgID, addr, err)
		return
	}
}

// checkValidityWithStateAndVerifySignature checks if the message is from active peer and matches corresponding
// sent request if a reply. In addition to what checkValidityWithState does, it also verifies the signature
// of the message. Should be called only with *signed* messages.
func checkValidityWithStateAndVerifySignature(sChan state.EventChanSend, msg []byte, msgID uint32, msgType byte, isReply bool, addr *net.UDPAddr) {
	// check with state whether the reply has corresponding request, obtain public key of the sender
	senderPubKey, err := state.CreateIncomingMessageEvent(sChan, msgID, msgType, isReply, addr)
	if err != nil {
		log.Printf("Failed to validate message with ID: %d from %s: %v", msgID, addr, err)
		return
	}

	// verify the signature of the message
	msgLen := ExtractMessageLen(msg)
	signedContentEnd := MsgBodyIndex + msgLen
	if !crypto.VerifySignature(senderPubKey, msg, signedContentEnd) {
		log.Printf("Signature verification failed for message with id: %d from %s, ignoring", msgID, addr)
		return
	}
}

func handlePingMessage(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, addr *net.UDPAddr, id uint32) {
	log.Printf("Received Ping from %s with ID: %d", addr, id)
	checkValidityWithState(sChan, id, Ping, false, addr)

	reply := BuildPingMessage(id, Ok)
	log.Printf("Scheduling to dispatch Ok to %s with ID: %d", addr, id)
	msgDisp.CreateDispatchMessageEvent(msgDispChan, reply, addr, false)
}

func handleHelloMessage(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend,
	addr *net.UDPAddr, msg []byte, id uint32, peerID string) {
	msgLen := ExtractMessageLen(msg)
	senderID := string(msg[MsgBodyIndex+ExtensionsSize : MsgBodyIndex+msgLen])

	log.Printf("Received Hello from %s and address: %s ", senderID, addr)

	senderKey := GetPeerPublicKeyFromServer(senderID)

	signedContentEnd := MsgBodyIndex + msgLen
	if !crypto.VerifySignature(senderKey, msg, signedContentEnd) {
		log.Printf("HelloReply signature verification failed for peer %s, ignoring", senderID)
		return
	}

	// Assume that sender will register itself with all the addresses
	state.RegisterNewPeer(sChan, senderID, addr, senderKey)

	reply := BuildHelloMessage(id, HelloReply, []byte(peerID))

	log.Printf("Scheduling to dispatch HelloReply to %s with ID: %d", addr, id)
	msgDisp.CreateDispatchMessageEvent(msgDispChan, reply, addr, true)
}

func handleErrorMessage(addr *net.UDPAddr, msg []byte) {
	log.Printf("Received error message from: %s", addr)

	id := binary.BigEndian.Uint32(msg[:MsgTypeIndex])
	errorMsg := string(msg[MsgBodyIndex:])
	log.Printf("Error message ID: %d, Content: %s", id, errorMsg)
}

func handleOkMessage(sChan state.EventChanSend, addr *net.UDPAddr, msgID uint32) {
	log.Printf("Received Ok message from %s with ID: %d", addr, msgID)

	checkValidityWithState(sChan, msgID, Ok, true, addr)
}

func handleHelloReply(sChan state.EventChanSend, addr *net.UDPAddr, msg []byte, msgID uint32) {
	log.Printf("Received HelloReply from %s with ID: %d", addr, msgID)

	msgLen := ExtractMessageLen(msg)
	msgSender := string(msg[MsgBodyIndex+ExtensionsSize : MsgBodyIndex+msgLen])

	senderKey := GetPeerPublicKeyFromServer(msgSender)

	signedContentEnd := MsgBodyIndex + msgLen
	if !crypto.VerifySignature(senderKey, msg, signedContentEnd) {
		log.Printf("Signature verification failed for HelloReply from peer %s", msgSender)
		return
	}

	// check with state whether HelloReply has corresponding Hello request
	state.ValidateHelloReplyAndRegisterNewPeer(sChan, msgID, addr, msgSender, senderKey)
}

func handleRootReply(sChan state.EventChanSend, addr *net.UDPAddr, msg []byte, msgID uint32) {
	log.Printf("Received RootReply message from %s with ID: %d", addr, msgID)
	checkValidityWithStateAndVerifySignature(sChan, msg, msgID, RootReply, true, addr)

	msgLen := ExtractMessageLen(msg)
	log.Printf("RootReply message length: %d", msgLen)
	state.RegisterRootHash(sChan, addr, msg[MsgBodyIndex:MsgBodyIndex+msgLen])
}

func handleNoDatumReply(sChan state.EventChanSend, addr *net.UDPAddr, msg []byte, msgID uint32) {
	log.Printf("Received NoDatum message from %s with ID: %d", addr, msgID)
	checkValidityWithStateAndVerifySignature(sChan, msg, msgID, NoDatum, true, addr)

	// inform iface through state or directly, that valid noDatum reply was received
}

func handleRootRequest(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, addr *net.UDPAddr, id uint32) {
	log.Printf("Received RootRequest from %s with ID: %d", addr, id)
	checkValidityWithState(sChan, id, RootRequest, true, addr)

	// TODO - we would here retrieve our RootHash from the data structure and pass it there.
	reply := BuildRootMessage(id, RootReply, "nil")

	log.Printf("Scheduling to dispatch RootReply to %s with ID: %d", addr, id)
	msgDisp.CreateDispatchMessageEvent(msgDispChan, reply, addr, true)
}

func handleDatumRequest(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, addr *net.UDPAddr, msg []byte, id uint32) {
	log.Printf("Received DatumRequest from %s with ID: %d", addr, id)
	checkValidityWithState(sChan, id, DatumRequest, false, addr)

	// TODO - we would here retrieve our Value from within the given hash in the request.
	// Verify whether the value with that hash exists and if not, send NoDatum reply.
	// Due to currently lack of this logic, we will reply with NoDatum all the time.
	msgLen := ExtractMessageLen(msg)
	dataHash := string(msg[MsgBodyIndex : MsgBodyIndex+msgLen])
	reply := BuildDataMessage(id, NoDatum, dataHash)

	log.Printf("Scheduling to dispatch NoDatum to %s with ID: %d", addr, id)
	msgDisp.CreateDispatchMessageEvent(msgDispChan, reply, addr, false)
}
