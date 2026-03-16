#include <stm32.h>
#include <irq.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

#include "definitions.h"

static char dma_buf[BUFF_SIZE];
static uint32_t dma_buf_len;

static char* to_send_buf[BUFF_SIZE];
static uint32_t to_send;

static void save_to_send_buffer(char* str) {
    if (to_send < BUFF_SIZE) {
        to_send_buf[to_send] = str;
        to_send++;
    }
}

static void save_to_dma_buffer() {
    for (int comm_idx = 0; comm_idx < to_send; comm_idx++) {
        
        // Stop if DMA buffer is full or would be overflown after adding another communicate
        if (dma_buf_len >= BUFF_SIZE || dma_buf_len + strlen(to_send_buf[comm_idx]) >= BUFF_SIZE) {
            break;
        } 

        strcpy(dma_buf + dma_buf_len, to_send_buf[comm_idx]);
        dma_buf_len = strlen(dma_buf);
    }
}

static void check_and_initiate_tx() {
    irq_level_t irq_level = IRQprotectAll();

    // If there is something to send and DMA is not busy, start sending
    if (to_send > 0 && (DMA1_Stream6->CR & DMA_SxCR_EN) == 0 && (DMA1->HISR & DMA_HISR_TCIF6) == 0) {      
        save_to_dma_buffer();

        DMA1_Stream6->M0AR = (uint32_t)dma_buf;
        DMA1_Stream6->NDTR = dma_buf_len;
        DMA1_Stream6->CR |= DMA_SxCR_EN;

        to_send = 0;
        dma_buf_len = 0;
    }

    IRQunprotectAll(irq_level);
}

static void handle_button(Button* btn) {
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
    if (strcmp("MODE", btn->name) == 0) { 
        save_to_send_buffer(current_state ? btn->press : btn->release);
    }
    else { 
        save_to_send_buffer(current_state ? btn->release : btn->press);
    }

    check_and_initiate_tx();
}

// Interruptions 
void DMA1_Stream6_IRQHandler() {
    // Read DMA1 interrupt status register
    uint32_t isr = DMA1->HISR;
    irq_level_t irq_level = IRQprotectAll();
    
    if (isr & DMA_HISR_TCIF6) {
        // Handle the completion of the transfer
        DMA1->HIFCR = DMA_HIFCR_CTCIF6;
        
        // If there is something to send, initiate next transfer.
        check_and_initiate_tx();
    }
    IRQunprotectAll(irq_level);
}

void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1U << USER_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR13;
        handle_button(&buttons[USER_BTN_POSITION]);
    }

    if (EXTI->PR & (1U << FIRE_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR10;
        handle_button(&buttons[FIRE_BTN_POSITION]);
    }
}

void EXTI3_IRQHandler(void) {
    if (EXTI->PR & (1U << LEFT_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR3;
        handle_button(&buttons[LEFT_BTN_POSITION]);
    }
}

void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1U << RIGHT_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR4;
        handle_button(&buttons[RIGHT_BTN_POSITION]);
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1U << UP_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR5;
        handle_button(&buttons[UP_BTN_POSITION]);
    }
    if (EXTI->PR & (1U << DOWN_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR6;
        handle_button(&buttons[DOWN_BTN_POSITION]);
    }
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1U << MODE_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR0;
        handle_button(&buttons[MODE_BTN_POSITION]);
    }
}

// Initial configuration
static void configure_system(void) {
    // Setup right GPIOs
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA1EN;

    // Setup USART
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Configure outside interruptions as on slides
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

static void configure_dma(void) {
    // USART2 Tx (Sending stream - stream no. 6, chan. 4)
    DMA1_Stream6->CR = 4U << 25 | DMA_SxCR_PL_1 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
    DMA1_Stream6->PAR = (uint32_t)&USART2->DR;

    // Clear markers and setup interruptions (activate DMA interruptions, but only on Tx channel)
    DMA1->HIFCR = DMA_HIFCR_CTCIF6;
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}

static void configure_interruptions(void) {
    // Configure GPIOs and configure EXTI as on slides
    // GPIOs
    GPIOinConfigure(USER_BTN_GPIO, USER_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(LEFT_BTN_GPIO, LEFT_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(RIGHT_BTN_GPIO, RIGHT_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(UP_BTN_GPIO, UP_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(DOWN_BTN_GPIO, DOWN_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(FIRE_BTN_GPIO, FIRE_BTN_PIN, GPIO_PuPd_UP, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);
    GPIOinConfigure(MODE_BTN_GPIO, MODE_BTN_PIN, GPIO_PuPd_DOWN, EXTI_Mode_Interrupt, EXTI_Trigger_Rising_Falling);

    // Zero the EXTI by saving 1 in according bits
    EXTI->PR |= (1U << USER_BTN_PIN); 
    EXTI->PR |= (1U << LEFT_BTN_PIN);
    EXTI->PR |= (1U << RIGHT_BTN_PIN);
    EXTI->PR |= (1U << UP_BTN_PIN);
    EXTI->PR |= (1U << DOWN_BTN_PIN);
    EXTI->PR |= (1U << FIRE_BTN_PIN);
    EXTI->PR |= (1U << MODE_BTN_PIN);

    // NVIC
    NVIC_EnableIRQ(EXTI15_10_IRQn); // USER BTN, FIRE BTN
    NVIC_EnableIRQ(EXTI3_IRQn);     // LEFT BTN
    NVIC_EnableIRQ(EXTI4_IRQn);     // RIGHT BTN
    NVIC_EnableIRQ(EXTI9_5_IRQn);   // UP, DOWN BTN
    NVIC_EnableIRQ(EXTI0_IRQn);     // MODE BTN
}

static void configure_uart(void) {
    GPIOafConfigure(GPIOA, 2, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_NOPULL, GPIO_AF_USART2);
    GPIOafConfigure(GPIOA, 3, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_UP, GPIO_AF_USART2);

    // Configure registers, as presented
    USART2->CR1 = USART_Mode_Rx_Tx | USART_WordLength_8b | USART_Parity_No;
    USART2->CR2 = USART_StopBits_1;
    USART2->CR3 = USART_CR3_DMAT; // setup only Tx in DMA
    USART2->BRR = (PCLK1_HZ + (BAUD / 2U)) / BAUD;

    // DMA configure
    configure_dma();

    // Configure buttons interruptions as on slides
    configure_interruptions();

    // Finally set UE in CR1
    USART2->CR1 |= USART_Enable;
}

int main(void) {
    // Set buffer
    memset(dma_buf, '\0', BUFF_SIZE);
    dma_buf_len = 0;

    memset(to_send_buf, '\0', BUFF_SIZE*sizeof(char*));
    to_send = 0;

    // Configure right GPIOs and say we use UART
    configure_system();

    // UART configure
    configure_uart();

    // We don't wait actively for events and print,
    // instead we implement the interruptions handling
    // and setup all neccessary things for buttons 
    // to cause interruptions and print changes via DMA
    while(true);

    return 0;
}
