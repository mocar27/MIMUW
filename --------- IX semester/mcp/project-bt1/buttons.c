/**
 * @file buttons.c
 * @brief Implementation of Buttons control functions for Project BT1 - Microcontrollers programming.
 */

#include <stm32.h>
#include <irq.h>
#include <stddef.h>
#include <string.h>

#include "buttons.h"

// Define sending configuration during compilation.
#if defined(USE_UART2)
    #define DMA_SEND_STREAM DMA1_Stream6
    #define DMA_SEND_FLAG DMA_HISR_TCIF6
    #define DMA_VERSION DMA1
#elif defined(USE_UART1)
    #define DMA_SEND_STREAM DMA2_Stream7
    #define DMA_SEND_FLAG DMA_HISR_TCIF7
    #define DMA_VERSION DMA2
#endif

// Global structures for buttons sending and debouncing.
Button buttons[] = {
    {USER_BTN_GPIO,  USER_BTN_PIN,  "USER",  "USER PRESSED\r\n",  "USER RELEASED\r\n"},
    {LEFT_BTN_GPIO,  LEFT_BTN_PIN,  "LEFT",  "LEFT PRESSED\r\n",  "LEFT RELEASED\r\n"},
    {RIGHT_BTN_GPIO, RIGHT_BTN_PIN, "RIGHT", "RIGHT PRESSED\r\n", "RIGHT RELEASED\r\n"},
    {UP_BTN_GPIO,    UP_BTN_PIN,    "UP",    "UP PRESSED\r\n",    "UP RELEASED\r\n"},
    {DOWN_BTN_GPIO,  DOWN_BTN_PIN,  "DOWN",  "DOWN PRESSED\r\n",  "DOWN RELEASED\r\n"},
    {FIRE_BTN_GPIO,  FIRE_BTN_PIN,  "FIRE",  "FIRE PRESSED\r\n",  "FIRE RELEASED\r\n"},
    {MODE_BTN_GPIO,  MODE_BTN_PIN,  "MODE",  "MODE PRESSED\r\n",  "MODE RELEASED\r\n"}
};

// Switch to statically binded value in buttons.h, as needed of usage in the main-bt1.c 
// #define BUTTONS_COUNT (sizeof(buttons) / sizeof(buttons[0]))

char dma_buf[BUFF_SIZE];
uint32_t dma_buf_len;

char* buffered_messages[BUFF_SIZE];
uint32_t buffered_count;

uint32_t debouncing_buttons[BUTTONS_COUNT];

/**
 * Function to configure all buttons and their interruptions at the start of the program.
 */
void setup_buttons(void) {
    // Configure GPIOs and EXTI as on slides
    // GPIOs
    GPIOinConfigure(USER_BTN_GPIO, USER_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(LEFT_BTN_GPIO, LEFT_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(RIGHT_BTN_GPIO, RIGHT_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(UP_BTN_GPIO, UP_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(DOWN_BTN_GPIO, DOWN_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(FIRE_BTN_GPIO, FIRE_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(MODE_BTN_GPIO, MODE_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);

    // Zero the EXTI by saving 1 in according bits
    EXTI->PR |= (1U << USER_BTN_PIN); 
    EXTI->PR |= (1U << LEFT_BTN_PIN);
    EXTI->PR |= (1U << RIGHT_BTN_PIN);
    EXTI->PR |= (1U << UP_BTN_PIN);
    EXTI->PR |= (1U << DOWN_BTN_PIN);
    EXTI->PR |= (1U << FIRE_BTN_PIN);
    EXTI->PR |= (1U << MODE_BTN_PIN);

    // Allow external interruptions for buttons
    NVIC_EnableIRQ(EXTI15_10_IRQn); // USER BTN, FIRE BTN
    NVIC_EnableIRQ(EXTI3_IRQn);     // LEFT BTN
    NVIC_EnableIRQ(EXTI4_IRQn);     // RIGHT BTN
    NVIC_EnableIRQ(EXTI9_5_IRQn);   // UP, DOWN BTN
    NVIC_EnableIRQ(EXTI0_IRQn);     // MODE BTN
}

/**
 * Function that is invoked after receiving external (button) interruption
 * to save given message in the buffer to later send it throught DMA.
 */
static void buffer_message(char* str) {
    // Save the pointer to the string, so we don't do unnecessary copies.
    if (buffered_count < BUFF_SIZE) {
        buffered_messages[buffered_count] = str;
        buffered_count++;
    }
}

/**
 * Function that copies buffered data to DMA buffer, 
 * for data to be available to send throught DMA.
 */
static void save_to_dma_buffer() {
    for (int msg_idx = 0; msg_idx < buffered_count; msg_idx++) {
        int msg_len = strlen(buffered_messages[msg_idx]);

        // Stop if DMA buffer is full or would be overflown after adding another communicate.
        if (dma_buf_len >= BUFF_SIZE || dma_buf_len + msg_len >= BUFF_SIZE) {
            break;
        }

        // Otherwise save to DMA buffer and later send it.
        strcpy(dma_buf + dma_buf_len, buffered_messages[msg_idx]);
        dma_buf_len += msg_len;
    }
}

void check_and_initiate_tx() {
    // If there is something to send and DMA is not busy, start sending
    if (buffered_count > 0 && (DMA_SEND_STREAM->CR & DMA_SxCR_EN) == 0 && (DMA_VERSION->HISR & DMA_SEND_FLAG) == 0) {      
        save_to_dma_buffer();

        DMA_SEND_STREAM->M0AR = (uint32_t)dma_buf;
        DMA_SEND_STREAM->NDTR = dma_buf_len;
        DMA_SEND_STREAM->CR |= DMA_SxCR_EN;

        buffered_count = 0;
        dma_buf_len = 0;
    }
}

void handle_button(Button* btn) {
    // At the time of invoking this function, 
    // we know which button caused the interruption

    // Joystick
    // LEFT PRESSED, LEFT RELEASED, 
    // RIGHT PRESSED, RIGH RELEASED, 
    // UP PRESSED, UP RELEASED, 
    // DOWN PRESSED, DOWN RELEASED, 

    // FIRE PRESSED, FIRE RELEASED, 
    // USER PRESSED, USER RELEASED, 
    // AT MODE PRESSED, AT MODE RELEASED;

    uint8_t current_state = (btn->gpio->IDR >> btn->pin) & 1 ? 1 : 0;
    
    // Buttons other than MODE are active low (pressed = 0)
    // As buttons have unique pins, compare the pin number upon encountering interruption,
    // as comparing of type strcmp("MODE", btn->name) is more expensie.
    if (btn->pin == MODE_BTN_PIN) { 
        buffer_message(current_state ? btn->press : btn->release);
    }
    else { 
        buffer_message(current_state ? btn->release : btn->press);
    }

    // After saving/receiving the external interruption of the message, check whether it can be sent.
    check_and_initiate_tx();
}
