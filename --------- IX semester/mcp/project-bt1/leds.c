/**
 * @file leds.c
 * @brief Implementation of LED control functions for Project BT1 - Microcontrollers programming.
 */

#include <stddef.h>
#include <stm32.h>

#include "leds.h"

// Define receiving stream during compilation
#if defined(USE_UART2)
    #define DMA_RECV_STREAM DMA1_Stream5
#elif defined(USE_UART1)
    #define DMA_RECV_STREAM DMA2_Stream2
#endif

// Global arrays for controlling LED operations
// Recv communicates on micro (changing lights of LEDs depending on the user input)
static char recv_buf[3];

LedLight leds[] = {
    {RED_LED_GPIO,    RED_LED_PIN,    RED_LED,    1, 0},
    {GREEN_LED_GPIO,  GREEN_LED_PIN,  GREEN_LED,  1, 0},
    {BLUE_LED_GPIO,   BLUE_LED_PIN,   BLUE_LED,   1, 0},
    {GREEN2_LED_GPIO, GREEN2_LED_PIN, GREEN2_LED, 0, 0}
};

#define LED_COUNT (sizeof(leds) / sizeof(leds[0]))

/**
 * Helper function to find the LedLight structure by its ID (char).
 * @param id The character ID of the LED.
 * @return Pointer to the LedLight structure element if found, NULL otherwise.
 */
static LedLight* find_led(char id) {
    for (int i = 0; i < LED_COUNT; i++) {
        if (leds[i].id == id) {
            return &leds[i];
        }
    }

    return NULL;
}

/**
 * Function to set the state of a given LED.
 * @param led Pointer to the LedLight elemenft from structure representing the LED.
 * @param new_state The new state to set (1 for ON, 0 for OFF).
 */
static void set_led_state(LedLight* led, uint8_t new_state) {
    led->state = new_state;

    if (new_state) { // If new_state is to turn given LED on
        if (led->active_low) {
            led->gpio->BSRR = (1 << (led->pin + LEDS_PIN_OFFSET));
        } else {
            led->gpio->BSRR = (1 << led->pin);
        }
    } else { // Otherwise it's to turn the LED off
        if (led->active_low) {
            led->gpio->BSRR = (1 << led->pin);
        } else {
            led->gpio->BSRR = (1 << (led->pin + LEDS_PIN_OFFSET));
        }
    }
}

/**
 * Function to configure all LEDs at the start of the program.
 */
void setup_leds(void) {
    for (int i = 0; i < LED_COUNT; i++) {
        // Switch off every LED at the beginning.
        set_led_state(&leds[i], 0);

        // Configure GPIOs for every LED.
        GPIOoutConfigure(leds[i].gpio, leds[i].pin, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    }
}

/**
 * Function to handle LED operations based on received commands.
 * @param led_id The character ID of the LED to operate on.
 * @param op The operation to perform ('1' for ON, '0' for OFF, 'T' for TOGGLE).
 */
void handle_led_operation(char led_id, char op) {
    LedLight* target_led = find_led(led_id);

    // If no such LED with given id is found, simply return -- discard user input.
    if (target_led == NULL) return;

    switch (op) {
        case CMD_LED_ON:
            set_led_state(target_led, 1);
            break;

        case CMD_LED_OFF:
            set_led_state(target_led, 0);
            break;

        case CMD_LED_TOGGLE:
            set_led_state(target_led, !target_led->state);
            break;
            
        default:
            break;
    }
}

/**
 * Function that is called in main function to initialize first DMA receiving transfer of user input.
 */
void initialize_user_input(void) {
    DMA_RECV_STREAM->M0AR = (uint32_t)recv_buf;
    DMA_RECV_STREAM->NDTR = 3;
    DMA_RECV_STREAM->CR |= DMA_SxCR_EN;
}

/**
 * Function to handle user input upon DMA interruption for LED lights changing. 
 */
void receive_user_input(void) {
    // Receive 3 characters from user input over UART/BT.
    // If received chars are correct, parse them and change the LED light.

    // If any instruction on the way of constructing the operation is incorrect,
    // handle_led_operation will ignore the LED switch instruction.
    
    // First char of instruction must be L
    if (recv_buf[0] == 'L') { 
        handle_led_operation(recv_buf[1], recv_buf[2]);
    }

    // At the end of processing input, we ask DMA to take another, 
    // new input in the recv_buf array.
    initialize_user_input();
}
