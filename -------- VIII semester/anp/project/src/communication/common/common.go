package common

import (
	"encoding/binary"
	"log"
	"net"

	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
)

func LogLineSeparator(s string) {
	log.Printf("------------------------------ %s ------------------------------", s)
}

func FilterIPv4(addresses []*net.UDPAddr) []*net.UDPAddr {
	var result []*net.UDPAddr
	for _, a := range addresses {
		if a.IP != nil && a.IP.To4() != nil {
			result = append(result, a)
		}
	}
	return result
}

func FilterIPv6(addresses []*net.UDPAddr) []*net.UDPAddr {
	var result []*net.UDPAddr
	for _, a := range addresses {
		if a.IP != nil && a.IP.To16() != nil && a.IP.To4() == nil {
			result = append(result, a)
		}
	}
	return result
}

func HandleUnknownMessage(msgDispChan msgDisp.EventChanSend, addr *net.UDPAddr, id uint32, msgType byte) {
	reply := BuildErrorMessage(id, "Unknown message type"+string(msgType))
	log.Printf("Sending error message for unknown type %d to %s", msgType, addr)
	msgDisp.CreateDispatchMessageEvent(msgDispChan, reply, addr, false)
}

func ExtractMessageLen(buf []byte) int {
	return int(binary.BigEndian.Uint16(buf[MsgLenIndex : MsgLenIndex+2]))
}
