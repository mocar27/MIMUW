package common

import (
	"bytes"
	"crypto/rand"
	"encoding/binary"
	"log"

	"github.com/Toomimi/advanced-networks/config"
)

// Build all needed messages for communication exchange.

// GenerateRandomID generates random message ID (4 bytes, uint32, 32-bits)
func GenerateRandomID() (msgID uint32) {
	if err := binary.Read(rand.Reader, binary.BigEndian, &msgID); err != nil {
		log.Fatalf("Failed to generate random ID: %v", err)
	}
	return msgID
}

// BuildHelloMessage constructs a Hello message to be sent to the server.
// Additionally, due to extended functionality and that Hello and HelloReply
// has the same structure, it can be reused for HelloReply message.
func BuildHelloMessage(msgID uint32, msgType byte, msgSender []byte) []byte {
	LogLineSeparator("buildHelloMessage")
	var buf bytes.Buffer

	if err := binary.Write(&buf, binary.BigEndian, msgID); err != nil {
		log.Fatalf("Failed to write ID: %v", err)
	}

	// type = 1 (1 byte, uint8, 8-bits)
	if err := buf.WriteByte(msgType); err != nil {
		log.Fatalf("Failed to write Type: %v", err)
	}

	// length of the body + extensions (2 bytes, uint16, 16-bits)
	if err := binary.Write(&buf, binary.BigEndian, uint16(len(msgSender)+4)); err != nil {
		log.Fatalf("Failed to write Length: %v", err)
	}

	// set extensions
	if err := binary.Write(&buf, binary.BigEndian, config.Extensions); err != nil {
		log.Fatalf("Failed to write Extensions: %v", err)
	}

	// write my name to the buffer
	if _, err := buf.Write(msgSender); err != nil {
		log.Fatalf("Failed to write Body: %v", err)
	}

	LogLineSeparator("buildHelloMessage")
	return buf.Bytes()
}

// BuildPingMessage constructs a Ping and Ping reply message.
func BuildPingMessage(msgID uint32, msgType byte) []byte {
	LogLineSeparator("buildPingMessage")
	var buf bytes.Buffer

	if err := binary.Write(&buf, binary.BigEndian, msgID); err != nil {
		log.Fatalf("Failed to write ID: %v", err)
	}
	log.Printf("Random ID: %d", msgID)

	// type (1 byte, uint8, 8-bits)
	if err := buf.WriteByte(msgType); err != nil {
		log.Fatalf("Failed to write Type: %v", err)
	}

	// length of message is 0
	if err := binary.Write(&buf, binary.BigEndian, uint16(0)); err != nil {
		log.Fatalf("Failed to write Length: %v", err)
	}

	LogLineSeparator("buildPingMessage")
	return buf.Bytes()
}

func BuildErrorMessage(msgID uint32, errorMsg string) []byte {
	LogLineSeparator("buildErrorMessage")
	var buf bytes.Buffer

	if err := binary.Write(&buf, binary.BigEndian, msgID); err != nil {
		log.Fatalf("Failed to write ID: %v", err)
	}

	// type (1 byte, uint8, 8-bits)
	if err := buf.WriteByte(Error); err != nil {
		log.Fatalf("Failed to write Type: %v", err)
	}

	// length of Body + Extensions (2 bytes, uint16, 16-bits)
	if err := binary.Write(&buf, binary.BigEndian, uint16(len(errorMsg))); err != nil {
		log.Fatalf("Failed to write Length: %v", err)
	}

	// write error message to the buffer
	if _, err := buf.Write([]byte(errorMsg)); err != nil {
		log.Fatalf("Failed to write Body: %v", err)
	}

	LogLineSeparator("buildErrorMessage")
	return buf.Bytes()
}

// BuildRootMessage builds both RootRequest and RootReply
func BuildRootMessage(msgID uint32, msgType byte, hash string) []byte {
	LogLineSeparator("buildRootMessage")
	var buf bytes.Buffer

	if err := binary.Write(&buf, binary.BigEndian, msgID); err != nil {
		log.Fatalf("Failed to write ID: %v", err)
	}

	if err := buf.WriteByte(msgType); err != nil {
		log.Fatalf("Failed to write Type: %v", err)
	}

	if err := binary.Write(&buf, binary.BigEndian, uint16(32)); err != nil {
		log.Fatalf("Failed to write Length: %v", err)
	}

	hashBytes := []byte(hash)
	if len(hashBytes) == 0 {
		return buf.Bytes()
	}

	// otherwise, write to the buffer
	if _, err := buf.Write(hashBytes); err != nil {
		log.Fatalf("Failed to write hash body: %v", err)
	}

	LogLineSeparator("buildRootMessage")
	return buf.Bytes()
}

// BuildDataMessage - builds both DatumRequest and NoDatum messages as their logic is the same
func BuildDataMessage(msgID uint32, msgType byte, hash string) []byte {
	LogLineSeparator("buildDataMessage")
	var buf bytes.Buffer

	if err := binary.Write(&buf, binary.BigEndian, msgID); err != nil {
		log.Fatalf("Failed to write ID: %v", err)
	}

	if err := buf.WriteByte(msgType); err != nil {
		log.Fatalf("Failed to write Type: %v", err)
	}

	if err := binary.Write(&buf, binary.BigEndian, uint16(32)); err != nil {
		log.Fatalf("Failed to write Length: %v", err)
	}

	hashBytes := []byte(hash)
	if len(hashBytes) == 0 {
		return buf.Bytes()
	}

	// otherwise, write to the buffer
	if _, err := buf.Write(hashBytes); err != nil {
		log.Fatalf("Failed to write hash body: %v", err)
	}

	LogLineSeparator("buildDataMessage")
	return buf.Bytes()
}

func BuildDatumReplyMessage(msgID uint32, hash string, value []byte) []byte {
	LogLineSeparator("buildDatumReplyMessage")
	var buf bytes.Buffer

	if err := binary.Write(&buf, binary.BigEndian, msgID); err != nil {
		log.Fatalf("Failed to write ID: %v", err)
	}

	if err := buf.WriteByte(Datum); err != nil {
		log.Fatalf("Failed to write Type: %v", err)
	}

	hashBytes := []byte(hash)
	if len(hashBytes) == 0 {
		return buf.Bytes()
	}

	if err := binary.Write(&buf, binary.BigEndian, uint16(len(hashBytes)+len(value))); err != nil {
		log.Fatalf("Failed to write Length: %v", err)
	}

	if _, err := buf.Write(hashBytes); err != nil {
		log.Fatalf("Failed to write hash body: %v", err)
	}

	if _, err := buf.Write(value); err != nil {
		log.Fatalf("Failed to write value body: %v", err)
	}

	LogLineSeparator("buildDatumReplyMessage")
	return buf.Bytes()
}
