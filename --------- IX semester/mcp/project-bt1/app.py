import serial
import time
import threading
import keyboard
import sys

SERIAL_PORT = '/dev/tty.HC-2010-01-06'  
BAUD_RATE = 9600

try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1.0)
    print(f"Connected with {SERIAL_PORT}")
except serial.SerialException:
    print(f"Unable to open port: {SERIAL_PORT}.")
    sys.exit()

def recv_from():
    while True:
        try:
            line = ser.readline().decode('utf-8').strip()
            if line:
                print(f"[STM32 -> PC]: {line}")
        except:
            break

# Data receiving from microcontroller thread 
recv_thread = threading.Thread(target=recv_from)
recv_thread.daemon = True
recv_thread.start()

def send_to():
    while True:
        data = input().strip()
        try: 
            ser.write(data.encode('utf-8'), end="")
            print(f"[PC -> STM32]: {data}")
        except:
            print("Error while sending/encoding data to STM32")

try:
    send_to()            
except KeyboardInterrupt:
    print("Keyboard interrupted.")
finally:
    ser.close()
