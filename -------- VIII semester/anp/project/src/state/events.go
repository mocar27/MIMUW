package state

import (
	"crypto/ecdsa"
	"net"
)

type stateEvent interface {
	isStateEvent() // marker
}

type eventChanRecv = <-chan stateEvent
type EventChanSend = chan<- stateEvent
type EventChan = chan stateEvent

type registerNewPeer struct {
	id        string
	address   *net.UDPAddr
	publicKey *ecdsa.PublicKey
}

func (registerNewPeer) isStateEvent() {}

type outgoingMsg struct {
	details ReqInfo
	retChan chan<- *outgoingMsgResp // buffered channel to respond, fire and forget approach;
}

func (outgoingMsg) isStateEvent() {}

type outgoingMsgResp struct {
	addr *net.UDPAddr
	err  error
}

type incomingMsg struct {
	reqID         uint32
	reqType       byte
	isReply       bool // whether the message is a reply to a request
	senderAddress *net.UDPAddr
	retChan       chan<- *incomingMsgResp // buffered channel to respond, fire and forget approach;
}

func (incomingMsg) isStateEvent() {}

type incomingHelloReplyMsg struct {
	reqID   uint32
	retChan chan<- *incomingMsgResp // buffered channel to respond, fire and forget approach
}

func (incomingHelloReplyMsg) isStateEvent() {}

// incomingMsgResp is used to respond to incomingMsg and incomingHelloReplyMsg
// state messages. In case of incomingHelloReplyMsg, only Err field is used and
// nil value, means that reply matches sent request.
type incomingMsgResp struct {
	PubKey *ecdsa.PublicKey
	Err    error // error if the reply is not valid
}

type registerRootHash struct {
	address  *net.UDPAddr
	rootHash string // the root hash to register for the peer
}

func (registerRootHash) isStateEvent() {}
