package _iface

import (
	"fmt"
	"golang.org/x/term"
	"log"
	"os"
	"sort"
	"strings"

	cm "github.com/Toomimi/advanced-networks/communication"
	"github.com/Toomimi/advanced-networks/communication/common"
	msgDisp "github.com/Toomimi/advanced-networks/communication/message-dispatcher"
	s "github.com/Toomimi/advanced-networks/state"
)

const (
	UpArrow     = 65
	DownArrow   = 66
	RightArrow  = 67
	LeftArrow   = 68
	ClearOutput = "\x1b[2J\x1b[H"
)

func IfaceHandler(sChan s.EventChanSend, msgDispChan msgDisp.EventChanSend, receiver s.IfaceChanRecv, peerName string) {
	currentActivePeers := make(map[string]string)
	keyChan := make(chan int)
	quitChan := make(chan struct{}) // signal for graceful shutdown
	currentPos := 0

	menuLevel := 0
	actionPos := 0
	actions := []string{"Action 1", "Action 2", "Action 3"}

	// handle any key presses
	go readKeyPress(keyChan, quitChan)

	// connect to peers active on server (we expect to receive HelloReply from them to display them)
	connectToPeers(sChan, msgDispChan, peerName)

	for {
		select {
		case msg := <-receiver:
			switch m := msg.(type) {
			case s.NewPeerIFace:
				if currentActivePeers[m.PeerID] == "" {
					// T: ALWAYS will be "", as this is after handskahe, we have to send root and wait here,
					// not got, retransmission, keep in iface state the status of obtaining the root,
					// e.g. "Requested..., Time-outed. Retransmission..., Couldn't obtain root"

					// send RootReqeust to obtain root hash of the peer
					currentActivePeers[m.PeerID] = "TempHashKey"

					if menuLevel == 0 {
						constructUserInterface(currentActivePeers, currentPos, menuLevel, actionPos, actions)
					}
				}
			case s.PeerExpiredIFace:
				delete(currentActivePeers, m.PeerID)
				if menuLevel == 0 {
					constructUserInterface(currentActivePeers, currentPos, menuLevel, actionPos, actions)
				}
			case s.PeerRootHash:
				//todo
				log.Printf("WIP: Peer %s sent root hash: %s", m.PeerID, m.RootHash)
			default:
				menuLevel = 0
				constructUserInterface(currentActivePeers, currentPos, menuLevel, actionPos, actions)
				fmt.Println("Unknown message type received in iface handler")
			}
		case key := <-keyChan:
			peerList := getSortedPeers(currentActivePeers)
			if menuLevel == 0 && len(peerList) > 0 {
				switch key {
				case UpArrow:
					if currentPos > 0 {
						currentPos--
					} else {
						currentPos = len(peerList) - 1 // Wrap around to the last peer
					}
				case DownArrow:
					if currentPos < len(peerList)-1 {
						currentPos++
					} else {
						currentPos = 0 // Wrap around to the first peer
					}
				case RightArrow:
					menuLevel = 1
					actionPos = 0
				}
			} else if menuLevel == 1 {
				switch key {
				case UpArrow:
					if actionPos > 0 {
						actionPos--
					}
				case DownArrow:
					if actionPos < len(actions)-1 {
						actionPos++
					}
				case LeftArrow:
					menuLevel = 0
				case RightArrow:
					peerID := peerList[currentPos]
					action := actions[actionPos]
					log.Printf("Performing '%s' on peer '%s'\n", action, peerID)
					// call action function
				}
			}
			constructUserInterface(currentActivePeers, currentPos, menuLevel, actionPos, actions)
		case <-quitChan:
			fmt.Print(ClearOutput)
			fmt.Println("Exiting program...")
			fmt.Println("\rUse Ctrl+C again to exit gracefully.\r")
			return
		default:
			continue
		}
	}
}

func ifaceLine() {
	fmt.Println("\r" + strings.Repeat("-", 60))
}

func readKeyPress(keyChan chan<- int, quitChan chan<- struct{}) {
	oldState, err := term.MakeRaw(int(os.Stdin.Fd()))
	if err != nil {
		panic(err)
	}
	defer func(fd int, oldState *term.State) {
		_ = term.Restore(fd, oldState)
	}(int(os.Stdin.Fd()), oldState)

	buf := make([]byte, 3)

	for {
		n, _ := os.Stdin.Read(buf)
		if n == 1 {
			switch buf[0] {
			case 'q': // press 'q' to quit
				quitChan <- struct{}{}
				return
			case 3: // ctrl+C (SIGINT)
				quitChan <- struct{}{}
				return
			}
		} else if n == 3 && buf[0] == 27 && buf[1] == 91 {
			// arrow keys
			keyChan <- int(buf[2])
		}
	}
}

func getSortedPeers(peers map[string]string) []string {
	var keys []string
	for k, _ := range peers {
		keys = append(keys, k)
	}
	sort.Strings(keys)
	return keys
}

func connectToPeers(sChan s.EventChanSend, msgDispChan msgDisp.EventChanSend, peerName string) {
	ps := cm.GetPeerList()
	for _, peerID := range ps {
		if peerID == common.ServerBaseAddress || peerID == peerName {
			continue
		}
		address, err := cm.GetPeerAddr(peerID)
		if err != nil {
			log.Printf("Failed to get address for peer %s: %v", peerID, err)
			continue
		}

		msgID := common.GenerateRandomID()
		if _, err = s.RegisterOutgoingRequest(sChan, msgID, peerID, common.Hello); err != nil {
			log.Printf("Failed to register outgoing request for peer %s: %v", peerID, err)
			continue
		}
		log.Printf("Scheduling for dispatch Hello message with ID: %d to %s at %s", msgID, peerID, address)
		msgDisp.CreateDispatchMessageEvent(msgDispChan, common.BuildHelloMessage(msgID, common.Hello, []byte(peerName)), address, true)
	}
}

func constructUserInterface(peers map[string]string, peerIDx int, menuLevel int, actionIDx int, actions []string) {
	fmt.Print(ClearOutput)

	peerList := getSortedPeers(peers)
	if menuLevel == 0 {
		fmt.Println("\rActive Peers (use arrows ↑↓ to navigate, → to select):")
		ifaceLine()
		for i, peer := range peerList {
			prefix := "   "
			if i == peerIDx {
				prefix = "-> "
			}
			fmt.Printf("\r%s%s\n", prefix, peer)
		}
	} else if menuLevel == 1 {
		selectedPeer := peerList[peerIDx]
		fmt.Printf("\rActions for peer '%s' (← to go back, ↑↓ to navigate, → to select):\n", selectedPeer)
		ifaceLine()
		for i, act := range actions {
			prefix := "   "
			if i == actionIDx {
				prefix = "-> "
			}
			fmt.Printf("\r%s%s\n", prefix, act)
		}
	}

	ifaceLine()
}
