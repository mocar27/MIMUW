package main

import (
	"crypto/ecdsa"
	"flag"
	"io"
	"log"
	"net"
	"os"

	cm "github.com/Toomimi/advanced-networks/communication"
	"github.com/Toomimi/advanced-networks/communication/common"
	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
	"github.com/Toomimi/advanced-networks/crypto"
	iface "github.com/Toomimi/advanced-networks/iface"
	"github.com/Toomimi/advanced-networks/state"
)

func setLogOutput(debug bool) {
	if !debug {
		log.SetOutput(io.Discard) // disable log output if not in debug mode
		return
	}

	logFile, err := os.OpenFile("logs.txt", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
	if err != nil {
		log.Fatalf("Failed to open log file: %v", err)
	}

	log.SetOutput(logFile)
}

func main() {
	numberOfAddresses := flag.Int("n", 1, "Number of addresses to register")
	peerName := flag.String("name", "sample-peer-name", "Name of the peer")
	debugFlag := flag.Bool("debug", false, "Enable debug mode")
	flag.Parse()

	setLogOutput(*debugFlag)

	common.LogLineSeparator("Peer Main")
	stateChan := make(state.EventChan, 10)
	ifaceChan := make(state.IfaceChan, 10)
	msgDispatcherChan := make(msgDisp.EventChan, 10)

	go state.HandleState(stateChan, ifaceChan, msgDispatcherChan)

	privateKey := crypto.GeneratePrivateKey()
	publicKey := crypto.ExtractPublicKey(privateKey)

	// Issue - when having multiple addresses, currently we use just the first binded connection and dispatcher
	// uses this first one, but initial hello - registering with the server has to happen for all addresses
	conn := startListeners(stateChan, msgDispatcherChan, *numberOfAddresses, publicKey, *peerName)
	go msgDisp.MessageDispatcher(msgDispatcherChan, conn, privateKey)
	iface.IfaceHandler(stateChan, msgDispatcherChan, ifaceChan, *peerName)

	// if sth needs sth from the state we use select (-stateChanReceiver, -timeout x seconds) not to block forever.

	// Hang main here, as we cannot let listeners end.
	// As there is no final termination signal of program, we hang forever.
	common.LogLineSeparator("Peer Main")
	select {}
}

func startListeners(stateChan state.EventChanSend, msgDispChan msgDisp.EventChanSend, numberOfAddresses int, pubKey *ecdsa.PublicKey, peerID string) *net.UDPConn {

	// publish my Public Key to the server and retrieve server address
	cm.PublishPublicKey(peerID, pubKey)
	serverAddr, err := cm.GetPeerAddr(common.ServerBaseAddress)
	if err != nil {
		log.Fatalf("Failed to get server address: %v", err)
	}

	conn, err := net.ListenUDP("udp", nil)
	if err != nil {
		log.Fatalf("Failed to bind UDP socket: %v", err)
	}

	go cm.ListenAndServe(stateChan, msgDispChan, conn, peerID)
	cm.RegisterAddressToServer(stateChan, msgDispChan, serverAddr, peerID)
	log.Printf("Registered port: %d for peer with ID: %s", conn.LocalAddr().(*net.UDPAddr).Port, peerID)
	for i := 1; i < numberOfAddresses; i++ {
		log.Printf("Multiple addresses not supported yet")
		// Each of additional addresses requires its own dispatcher, interface would use the first
		// one and thus just the first address msgDispChan could be returned from this func

		break
	}
	return conn
}
