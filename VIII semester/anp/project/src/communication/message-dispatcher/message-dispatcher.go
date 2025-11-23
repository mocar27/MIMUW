package message_dispatcher

import (
	"crypto/ecdsa"
	"log"
	"net"

	"github.com/Toomimi/advanced-networks/crypto"
)

type eventChanRecv <-chan toDispatchEvent
type EventChanSend = chan<- toDispatchEvent
type EventChan = chan toDispatchEvent
type toDispatchEvent struct {
	message           []byte
	targetAddr        *net.UDPAddr
	requiresSignature bool // if true, the message will be signed
}

func CreateDispatchMessageEvent(sender EventChanSend, msg []byte, addr *net.UDPAddr, requiresSignature bool) {
	sender <- toDispatchEvent{
		message:           msg,
		targetAddr:        addr,
		requiresSignature: requiresSignature,
	}
}

func MessageDispatcher(receiver eventChanRecv, conn *net.UDPConn, key *ecdsa.PrivateKey) {
	for {
		event := <-receiver
		log.Println("MessageDispatcher received dispatch request: ", event)
		msg := event.message
		if event.requiresSignature {
			msg = crypto.SignMessage(key, msg)
		}

		if _, err := conn.WriteToUDP(msg, event.targetAddr); err != nil {
			log.Printf("Failed to dispatch message to %s: %v", event.targetAddr, err)
		} else {
			log.Printf("Message dispatched to %s", event.targetAddr)
		}

	}
}
