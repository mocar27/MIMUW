# Embedded Systems - Project BT1 (Microcontrollers programming)

Objective of this project is to implement a `Game Console` controller using an STM32 microcontroller (specifically the STM32F411RET6). The system facilitates bidirectional communication between the microcontroller and PC via Bluetooth or USB cable. The controller is equipped with buttons and LEDs, allowing user to interact with buttons and switch LED lights.

## Functional requirements

Base requirements of the project are:

- **Bluetooth Communication**: establishes a wireless connection between the microcontroller and PC via simple Python script using a Bluetooth module (HC-06) installed on the Nucleo board to demonstrate input/output capabilities.
- **LED Control (Rx)**: Receive control messages from the external device to toggle the microcontroller's on-board LEDs.
- **Joystick Input (Tx)**: Detect joystick activity and send messages to the external device for every button press and release event.

Additional requirements are:

- **DMA & Interrupts**: Communication between the microcontroller and the Bluetooth module is handled entirely using DMA (Direct Memory Access) and interrupts to offload the CPU.
- **External Interrupts**: Joystick activity is detected using external interrupts (EXTI).
- **Debouncing**: Button contact debouncing is implemented using a hardware timer and its associated interrupts.

## Compilation and running

Project uses Makefile for compilation and during compilation defines,
whether it should be run on the microcontroller-PC interaction throught USB cable or through Bluetooth. 
Initially it's defined to work through USB through `minicom` program (UART set to 2 during compilation by default,
to make it work through BT invoke `make UART=1`). Project was written and compiled on Linux system (Debian), 
which is installed on the students computer laboratories at the MIMUW Faculty.

After compilation project has to be uploaded to the microcontroller. It was done in the students laboratories 
with designated script, that was already installed on the computers. If you want to upload it manually and setup 
`GCC`, `Binutils`, `GDB`, `Newlib`, `CMSIS`, `OpenOCD` and `minicom`.

## References

- [STM32F411 Datasheet](https://www.st.com/resource/en/datasheet/stm32f411ce.pdf)
- [STM32F411 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0383-stm32f411xce-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DMA & BT with it, MIMUW slides](https://www.mimuw.edu.pl/~marpe/mikrokontrolery/w8_dma.pdf)
- [Microcontrollers programming course page](https://www.mimuw.edu.pl/~marpe/mikrokontrolery/)