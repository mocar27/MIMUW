#ifndef LEDS_H
#define LEDS_H

#include <gpio.h>

// LEDs definitions
#define LEDS_PIN_OFFSET 16

#define RED_LED_GPIO    GPIOA
#define GREEN_LED_GPIO  GPIOA
#define BLUE_LED_GPIO   GPIOB
#define GREEN2_LED_GPIO GPIOA

#define RED_LED_PIN    6
#define GREEN_LED_PIN  7
#define BLUE_LED_PIN   0
#define GREEN2_LED_PIN 5

#define RED_LED    'R'
#define GREEN_LED  'G'
#define BLUE_LED   'B'
#define GREEN2_LED 'g'

#define CMD_LED_INDICATOR 'L'
#define CMD_LED_OFF       '0'
#define CMD_LED_ON        '1'
#define CMD_LED_TOGGLE    'T'

// LEDs typedef and state storing
typedef struct LedLight {
    GPIO_TypeDef* gpio;
    uint8_t pin;
    char id;
    uint8_t active_low;
    uint8_t state;
} LedLight;

extern LedLight leds[]; 

// LEDs functions declarations
void setup_leds(void);
void handle_led_operation(char led_id, char op);
void receive_user_input(void);
void initialize_user_input(void);

#endif // LEDS_H
