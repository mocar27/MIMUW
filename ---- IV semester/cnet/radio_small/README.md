# Radio small - Task 1

The task of implementing an Internet radio was divided into two independent parts - implementing a transmitter and implementing a receiver.

The project, especially receiver was implemented using concurrent programming to receive and play data at the same time, so it's nonblocking and radio plays more fluently.

The project is fully implemented in C programming language.

## Part A (transmitter)

The transmitter is designed to send the data stream received on the standard input to the receiver. 
The transmitter should send the data received on the standard input packed in UDP datagrams to the **DATA_PORT** port to the **DEST_ADDR** address indicated in the command line.
Data should be sent in packets of **PSIZE** bytes, according to the protocol described below.

## Part B (receiver)

The receiver receives the data sent by the transmitter and outputs it to the standard output.

The receiver has a buffer of **BSIZE** bytes, designed to store data from a maximum of **⌊BSIZE/PSIZE⌋** subsequent packets.

When starting playback, the receiver:
- Clears the buffer, in particular discarding data contained in it but not yet output to standard output.
- It connects to the transmitter via the **DATA_PORT** port. The address of the receiver to which the transmitter sends data is fixed.
- After receiving the first audio packet, it saves the value of the **session_id** field and the number of the first received byte, let's call it **BYTE0**.
- Until a byte number **BYTE0 + ⌊BSIZE*3/4⌋** or greater is received, the receiver does not transmit data to the standard output.

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
