package state

type IfaceEvent interface {
	isIfaceEvent() // marker
}

type ifaceChanSend = chan<- IfaceEvent
type IfaceChanRecv = <-chan IfaceEvent
type IfaceChan = chan IfaceEvent

type NewPeerIFace struct {
	PeerID string
}

func (NewPeerIFace) isIfaceEvent() {}

type PeerExpiredIFace struct {
	PeerID string
}

func (PeerExpiredIFace) isIfaceEvent() {}

type PeerRootHash struct {
	PeerID   string
	RootHash string
}

func (PeerRootHash) isIfaceEvent() {}
