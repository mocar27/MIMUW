// definitions.h
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <gpio.h>

// CR1
#define USART_Mode_Rx_Tx (USART_CR1_RE | USART_CR1_TE)
#define USART_Enable USART_CR1_UE
// CR1 WordLen
#define USART_WordLength_8b 0x0000
#define USART_WordLength_9b USART_CR1_M
// CR1 parity
#define USART_Parity_No 0x0000
#define USART_Parity_Even USART_CR1_PCE
#define USART_Parity_Odd (USART_CR1_PCE | USART_CR1_PS)

// CR2 stop bits
#define USART_StopBits_1 0x0000
#define USART_StopBits_0_5 0x1000
#define USART_StopBits_2 0x2000
#define USART_StopBits_1_5 0x3000

// CR3 flow control
#define USART_FlowControl_None 0x0000
#define USART_FlowControl_RTS USART_CR3_RTSE
#define USART_FlowControl_CTS USART_CR3_CTSE

// BRR 
#define HSI_HZ 16000000U
#define PCLK1_HZ HSI_HZ
#define BAUD 9600U

// Define GPIOs for leds and buttons
#define USER_BTN_GPIO  GPIOC
#define LEFT_BTN_GPIO  GPIOB
#define RIGHT_BTN_GPIO GPIOB
#define UP_BTN_GPIO    GPIOB
#define DOWN_BTN_GPIO  GPIOB
#define FIRE_BTN_GPIO  GPIOB
#define MODE_BTN_GPIO  GPIOA

// Define Outs
#define USER_BTN_PIN  13
#define LEFT_BTN_PIN  3
#define RIGHT_BTN_PIN 4
#define UP_BTN_PIN    5
#define DOWN_BTN_PIN  6
#define FIRE_BTN_PIN  10
#define MODE_BTN_PIN  0

// Define positions in Buttons array
#define USER_BTN_POSITION 0
#define LEFT_BTN_POSITION 1
#define RIGHT_BTN_POSITION 2
#define UP_BTN_POSITION 3
#define DOWN_BTN_POSITION 4
#define FIRE_BTN_POSITION 5
#define MODE_BTN_POSITION 6

// Buttons pressing logic
typedef struct Button {
    GPIO_TypeDef* gpio;
    uint8_t pin;
    char* name;
    char* press;
    char* release;
} Button;

Button buttons[] = {
    {USER_BTN_GPIO, USER_BTN_PIN, "USER", "USER PRESSED\r\n", "USER RELEASED\r\n"},
    {LEFT_BTN_GPIO, LEFT_BTN_PIN, "LEFT", "LEFT PRESSED\r\n", "LEFT RELEASED\r\n"},
    {RIGHT_BTN_GPIO, RIGHT_BTN_PIN, "RIGHT", "RIGHT PRESSED\r\n", "RIGHT RELEASED\r\n"},
    {UP_BTN_GPIO, UP_BTN_PIN, "UP", "UP PRESSED\r\n", "DOWN RELEASED\r\n"},
    {DOWN_BTN_GPIO, DOWN_BTN_PIN, "DOWN", "DOWN PRESSED\r\n", "DOWN RELEASED\r\n"},
    {FIRE_BTN_GPIO, FIRE_BTN_PIN, "FIRE", "FIRE PRESSED\r\n", "FIRE RELEASED\r\n"},
    {MODE_BTN_GPIO, MODE_BTN_PIN, "MODE", "MODE PRESSED\r\n", "MODE RELEASED\r\n"}
};

#define BUTTONS_COUNT (sizeof(buttons) / sizeof(buttons[0]))
#define BUFF_SIZE 1024
#define MAX_COMMUNICATE_LEN 32

#endif // DEFINITIONS_H