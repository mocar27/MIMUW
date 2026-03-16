#ifndef BUTTONS_H
#define BUTTONS_H

#include <gpio.h>

// Buttons definitions
#define USER_BTN_GPIO  GPIOC
#define LEFT_BTN_GPIO  GPIOB
#define RIGHT_BTN_GPIO GPIOB
#define UP_BTN_GPIO    GPIOB
#define DOWN_BTN_GPIO  GPIOB
#define FIRE_BTN_GPIO  GPIOB
#define MODE_BTN_GPIO  GPIOA

#define USER_BTN_PIN  13
#define LEFT_BTN_PIN  3
#define RIGHT_BTN_PIN 4
#define UP_BTN_PIN    5
#define DOWN_BTN_PIN  6
#define FIRE_BTN_PIN  10
#define MODE_BTN_PIN  0

#define USER_BTN_POSITION 0
#define LEFT_BTN_POSITION 1
#define RIGHT_BTN_POSITION 2
#define UP_BTN_POSITION 3
#define DOWN_BTN_POSITION 4
#define FIRE_BTN_POSITION 5
#define MODE_BTN_POSITION 6

#define BUFF_SIZE 1024
#define BUTTONS_COUNT 7
// #define BUTTONS_COUNT (sizeof(buttons) / sizeof(buttons[0]))

// Buttons typedef and pressing logic
typedef struct Button {
    GPIO_TypeDef* gpio;
    uint8_t pin;
    char* name;
    char* press;
    char* release;
} Button;

extern Button buttons[];
extern char dma_buf[];
extern uint32_t dma_buf_len;

extern char* buffered_messages[];
extern uint32_t buffered_count;

extern uint32_t debouncing_buttons[];

// Buttons functions declarations
void setup_buttons(void);
void handle_button(Button* btn); 

void check_and_initiate_tx();

#endif // BUTTONS_H
