package communication

import (
	"bytes"
	"crypto/ecdsa"
	"errors"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
	"strings"

	. "github.com/Toomimi/advanced-networks/communication/common"
	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
	"github.com/Toomimi/advanced-networks/crypto"
	"github.com/Toomimi/advanced-networks/state"
)

// GetPeerList - 3.1 obtains list of peers from the server.
// The server replies with a 200 reply with a list of peer names, one per line.
func GetPeerList() []string {
	url := ServerBaseURL + "/peers/"
	LogLineSeparator("GetPeerList")
	log.Printf("Fetching peer list")

	resp, err := crypto.SecureClient().Get(url)
	if err != nil {
		log.Fatalf("GET request failed: %v", err)
	}
	defer func(Body io.ReadCloser) {
		_ = Body.Close()
	}(resp.Body)

	if resp.StatusCode != http.StatusOK {
		log.Fatalf("Unexpected server status: %s", resp.Status)
	}
	log.Printf("Received response with status: %s", resp.Status)

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("Failed to read response body: %v", err)
	}

	log.Printf("Successfully fetched peer list")
	LogLineSeparator("GetPeerList")
	lines := strings.Split(strings.TrimSpace(string(body)), "\n")
	return lines
}

// PublishPublicKey - 3.2 in order to register with the server, a peer peerID makes a PUT request
// to the server URL, then it communicates its presence and addresses.
func PublishPublicKey(peerID string, pubKey *ecdsa.PublicKey) {
	url := fmt.Sprintf("%s/peers/%s/key", ServerBaseURL, peerID)
	LogLineSeparator("PublishPublicKey")

	bytesKey := crypto.PublicKeyToBytes(pubKey)
	req, err := http.NewRequest("PUT", url, bytes.NewReader(bytesKey))
	if err != nil {
		log.Fatalf("Failed to create request: %v", err)
	}
	req.Header.Set("Content-Type", "application/octet-stream")
	log.Printf("Sending PUT request to %s with public ECDSA key", url)

	resp, err := crypto.SecureClient().Do(req)
	if err != nil {
		log.Fatalf("PUT request failed: %v", err)
	}
	defer func(Body io.ReadCloser) {
		_ = Body.Close()
	}(resp.Body)

	// server returns nothing on success, so we expect a 204 No Content status code.
	if resp.StatusCode != http.StatusNoContent {
		fmt.Println("Provided peer name is already registered with the server, try different one.")
		log.Fatalf("Server returned unexpected status: %s", resp.Status)
	}

	LogLineSeparator("PublishPublicKey")
}

func GetPeerAddr(peerID string) (*net.UDPAddr, error) {
	LogLineSeparator("GetPeerAddr")

	serverAddresses := GetPeerAddresses(peerID)
	if len(serverAddresses) == 0 {
		log.Printf("No peer addresses found for: %s", peerID)
		return nil, errors.New("no peer addresses found")
	}

	ipV4Addresses := FilterIPv4(serverAddresses)
	if len(ipV4Addresses) == 0 {
		log.Printf("No IPv4 peer addresses found for: %s", peerID)

		// fallback for IPv6 if no IPv4 addresses were found.
		ipV6Addresses := FilterIPv6(serverAddresses)
		if len(ipV6Addresses) == 0 {
			log.Printf("No IPv6 peer addresses found for: %s", peerID)
			return nil, errors.New("no peer addresses found")
		}
		log.Printf("Obtained peer address: %s for %s", ipV6Addresses[0], peerID)
		return ipV6Addresses[0], nil
	}

	log.Printf("Obtained peer address: %s for %s", ipV4Addresses[0], peerID)
	LogLineSeparator("GetPeerAddr")
	return ipV4Addresses[0], nil
}

func RegisterAddressToServer(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, serverAddr *net.UDPAddr, peerID string) {
	LogLineSeparator("RegisterAddressToServer")
	log.Printf("Registering peer with ID: %s by sending initial Hello request", peerID)

	registerToStateAndSendInitialHello(sChan, msgDispChan, serverAddr, peerID)

	LogLineSeparator("RegisterAddressToServer")
}

// GetPeerPublicKeyFromServer - 3.3 obtains PublicKey of peerID peer.
// Server replies with 200 reply with a 64-byte public key.
func GetPeerPublicKeyFromServer(peerID string) *ecdsa.PublicKey {
	LogLineSeparator("GetPeerPublicKeyFromServer")
	url := fmt.Sprintf("%s/peers/%s/key", ServerBaseURL, peerID)
	log.Printf("Fetching public key for peer ID: %s", peerID)

	resp, err := crypto.SecureClient().Get(url)
	if err != nil {
		log.Fatalf("GET request failed: %v", err)
	}
	defer func(Body io.ReadCloser) {
		_ = Body.Close()
	}(resp.Body)

	if resp.StatusCode != http.StatusOK {
		log.Fatalf("Unexpected status code: %s", resp.Status)
	}
	log.Printf("Received response with status: %s", resp.Status)

	keyBytes, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("Failed to read public key: %v", err)
	}

	if len(keyBytes) != KeyLen64 {
		log.Fatalf("Invalid public key size: received %d bytes, but should be 64 bytes", len(keyBytes))
	}
	parsedKey := crypto.ParsePublicKey(keyBytes)

	log.Printf("Successfully fetched public key for peer ID: %s", peerID)
	LogLineSeparator("GetPeerPublicKeyFromServer")
	return parsedKey
}

// GetPeerAddresses - 3.4 obtains addresses of peerID peer.
// The server replies with a list of UDP socket addresses, one per line.
func GetPeerAddresses(peerID string) []*net.UDPAddr {
	LogLineSeparator("GetPeerAddresses")
	url := fmt.Sprintf("%s/peers/%s/addresses", ServerBaseURL, peerID)
	log.Printf("Fetching addresses for peer ID: %s", peerID)

	resp, err := crypto.SecureClient().Get(url)
	if err != nil {
		log.Fatalf("GET request failed: %v", err)
	}
	defer func(Body io.ReadCloser) {
		_ = Body.Close()
	}(resp.Body)

	if resp.StatusCode != http.StatusOK && resp.StatusCode != http.StatusNoContent {
		log.Fatalf("Unexpected status: %s", resp.Status)
	}

	body, err := io.ReadAll(resp.Body)
	if err != nil {
		log.Fatalf("Failed to read response body: %v", err)
	}

	lines := strings.Split(strings.TrimSpace(string(body)), "\n")
	var results []*net.UDPAddr
	for _, line := range lines {
		addr, err := net.ResolveUDPAddr("udp", line)
		if err != nil {
			log.Fatalf("Failed to resolve address %q: %v", line, err)
		}
		log.Printf("Resolved address: %s for peer ID: %s:", addr, peerID)
		results = append(results, addr)
	}

	log.Printf("Successfully fetched addresses for peer ID: %s", peerID)
	LogLineSeparator("GetPeerAddresses")
	return results
}

func registerToStateAndSendInitialHello(sChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, serverAddr *net.UDPAddr, peerName string) {
	msgID := GenerateRandomID()
	// targetPeerID is unimportant in initial Hello
	_, err := state.RegisterOutgoingRequest(sChan, msgID, "", Hello)
	if err != nil {
		log.Fatalf("Failed to register outgoing request for Hello message: %v", err)
	}
	msg := BuildHelloMessage(msgID, Hello, []byte(peerName))
	log.Printf("Scheduling for dispatch initial Hello message with ID: %d to server at %s", msgID, serverAddr)
	msgDisp.CreateDispatchMessageEvent(msgDispChan, msg, serverAddr, true)
}
