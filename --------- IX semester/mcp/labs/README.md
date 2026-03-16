# Microcontrollers programming labs

## Lab0 Task

The objective of the task was to finish the implementation of the `Hello World` on the microcontroller, which means to blink the LED on the board.

The program in the infinite loop should have disply a sequence od blinking LED lights, such as:
- `Red` light for a moment,
- `Green` light for a moment,
- `Blue` light for a moment,
- `Small Green` light for a moment.

Important thing is that, after clicking and releasing the reset button, the very first sequence should be correct.

## Lab1 Task

The objective of the task was to implement communication between the microcontroller and the computer in the lab through the USB port using UART.

There should be a possibility to receive communicates on the microcontroller and control the LED lights using the following commands:
- LR1, LR0, LRT;
- LG1, LG0, LGT;
- LB1, LB0, LBT;
- Lg1, Lg0, LgT.

Additionally, there should be possibility to send communicates from the microcontroller to the computer, that would indicate the buttons state, such as:
- LEFT PRESSED, LEFT RELEASED;
- RIGHT PRESSED, RIGHT RELEASED;
- UP PRESSED, UP RELEASED; 
- DOWN PRESSED, DOWN RELEASED; 
- FIRE PRESSED, FIRE RELEASED; 
- USER PRESSED, USER RELEASED; 
- MODE PRESSED, MODE RELEASED.

Incorrect commands should be ignored. Receiving and sending communicates should not interrupt each other. 
The detection of multiple click can be ignored.

## Lab2 Task

The task objective is the same as in the **Lab1 Task**, but this time the communication should be implemented using
**DMA and its interruptions**. To handle the button clicking the **external interruptions** should be used.
