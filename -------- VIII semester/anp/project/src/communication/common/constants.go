package common

const (
	ServerBaseURL     = "https://galene.org:8448"
	ServerBaseAddress = "galene.org"
	MsgTypeIndex      = 4
	MsgLenIndex       = 5
	MsgBodyIndex      = 7
	KeyLen64          = 64
	ExtensionsSize    = 4

	Ping                 = 0
	Hello                = 1
	RootRequest          = 2
	DatumRequest         = 3
	NatTraversalRequest  = 4
	NatTraversalRequest2 = 5
	Ok                   = 128
	Error                = 129
	HelloReply           = 130
	RootReply            = 131
	Datum                = 132
	NoDatum              = 133
)

func CheckIfRequestAndReply(reqType, replyType byte) bool {
	switch reqType {
	case Ping:
		return replyType == Ok
	case Hello:
		return replyType == HelloReply
	case RootRequest:
		return replyType == RootReply
	case DatumRequest:
		return replyType == Datum || replyType == NoDatum
	default:
		return false
	}
}
