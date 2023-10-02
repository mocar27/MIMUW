# Radio big - Task 2

The task of implementing an Internet radio was divided into two independent parts - implementing a transmitter and implementing a receiver.

The project, receiver as well as transmitter were implemented using concurrent programming to send, receive and play data at the same time, so it's nonblocking and radio plays more fluently.

The project is fully implemented in C programming language.

## Part A (transmitter)

The transmitter is used to send the data stream received on the standard input to the receivers. 
The transmitter should send this data packed in UDP datagrams to the **DATA_PORT** port to the **MCAST_ADDR** directed broadcast address indicated on the command line.
Data should be sent in packets of **PSIZE** bytes, according to the protocol described below.

The transmitter listens using UDP on the **CTRL_PORT** port, accepting broadcast packets and recognizing the message:
- LOOKUP (identification requests): this is immediately responded to with a REPLY message in accordance with the protocol specifications below.

## Part B (receiver)

The receiver receives the data sent by the transmitter and outputs it to the standard output.

The receiver sends an identification request (LOOKUP message) to the **DISCOVER_ADDR** address on the **CTRL_PORT** port approximately every 5 seconds. 
Based on the received responses (REPLY messages), it creates a list of available radio stations. 
A station from which the receiver has not received a REPLY message for 20 seconds is removed from the list.

The receiver has a buffer of **BSIZE** bytes, designed to store data from a maximum of **⌊BSIZE/PSIZE⌋** subsequent packets.

When starting playback, the receiver:
- Clears the buffer, in particular discarding data contained in it but not yet output to standard output.
- It connects to the transmitter via the **DATA_PORT** port. The address of the receiver to which the transmitter sends data is fixed.
- After receiving the first audio packet, it saves the value of the **session_id** field and the number of the first received byte, let's call it **BYTE0**.
- Until a byte number **BYTE0 + ⌊BSIZE*3/4⌋** or greater is received, the receiver does not transmit data to the standard output.

The receiver expects TCP connections on the **UI_PORT** port. 
If the user connects there, e.g. using telnet, they will see a simple text interface in which the stations can be changed using the up/down arrows (without having to press Enter). 
If there are several connections, they all display the same thing and changes in one of them also occur in the other. 
The displayed station list updates when new stations are detected or unavailable ones are dropped. 
It looks exactly like this:

'------------------------------------------------------------------------'

 SIK Radio

'------------------------------------------------------------------------'

PR1

Radio "357"

 '>' Radio "Disco Pruszkow"

'------------------------------------------------------------------------'

## Audio data transfer protocol

Data exchange: data exchange takes place over UDP. Communication is one-way - the transmitter sends audio packets and the receiver receives them.

Datagram Format: Datagrams transmit binary data.

Byte order: all numbers are sent in network byte order (big-endian).

Audio pack:
- uint64 session_id
- uint64 first_byte_num
- byte[] audio_data
 
The **session_id** field is constant for the entire time the transmitter is launched.

The receiver remembers the **session_id** value from the first packet it received after starting playback, if it receive a packet with:
- smaller **session_id**, receiver ignores it
- larger **session_id**, receiver starts playing again from the beginning from new transmitter

## Control messages transfer protocol

Data is exchanged via UDP.

Datagrams transmit text data in the format described below (exluding: "):
- The LOOKUP message looks like this: "ZERO_SEVEN_COME_IN\n"
- The REPLY message looks like this: "BOREWICZ_HERE [MCAST_ADDR] [DATA_PORT] [station name]"
