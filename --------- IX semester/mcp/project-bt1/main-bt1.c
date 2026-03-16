/**
 * @file main-bt1.c
 * @brief Main file for Project BT1 - Microcontrollers programming.
 */

#include <stdbool.h>
#include <string.h>
#include <irq.h>

#include "config.h"
#include "leds.h"
#include "buttons.h"

#if defined(USE_UART1)
void DMA2_Stream2_IRQHandler(void) {
	// Read DMA2 interrupt status register.
    uint32_t isr = DMA2->LISR;

    if (isr & DMA_LISR_TCIF2) {
        // Handle the completion of the transfer.
        DMA2->LIFCR = DMA_LIFCR_CTCIF2;
    	
        // If there is something to receive, read it.
        receive_user_input();
    }
}

void DMA2_Stream7_IRQHandler() {
    // Read DMA2 interrupt status register
    uint32_t isr = DMA2->HISR;

    if (isr & DMA_HISR_TCIF7) {
        // Handle the completion of the transfer
        DMA2->HIFCR = DMA_HIFCR_CTCIF7;
        
        // If there is something to send, initiate next transfer.
        check_and_initiate_tx();
    }
}
#elif defined(USE_UART2)
void DMA1_Stream5_IRQHandler(void) {
	// Read DMA1 interrupt status register.
    uint32_t isr = DMA1->HISR;

    if (isr & DMA_HISR_TCIF5) {
        // Handle the completion of the transfer.
        DMA1->HIFCR = DMA_HIFCR_CTCIF5;    	
        
        // If there is something to receive, read it.
        receive_user_input();
    }
}

void DMA1_Stream6_IRQHandler() {
    // Read DMA1 interrupt status register
    uint32_t isr = DMA1->HISR;

    if (isr & DMA_HISR_TCIF6) {
        // Handle the completion of the transfer
        DMA1->HIFCR = DMA_HIFCR_CTCIF6;
        
        // If there is something to send, initiate next transfer.
        check_and_initiate_tx();
    }
}
#endif

/**
 * External interruptions for buttons 
 * EXTI15_10_IRQHandler - for USER and FIRE buttons.
 * EXTI3_IRQHandler - for LEFT button.
 * EXTI4_IRQHandler - for RIGHT button.
 * EXTI9_5_IRQHandler - for UP and DOWN buttons.
 * EXTI0_IRQHandler - for MODE button.
 */
void EXTI15_10_IRQHandler(void) {
    if (EXTI->PR & (1U << USER_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR13;

        if (debouncing_buttons[USER_BTN_POSITION] == 0) {
            debouncing_buttons[USER_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[USER_BTN_POSITION]);
        }
    }

    if (EXTI->PR & (1U << FIRE_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR10;

        if (debouncing_buttons[FIRE_BTN_POSITION] == 0) {
            debouncing_buttons[FIRE_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[FIRE_BTN_POSITION]);
        }
    }
}

void EXTI3_IRQHandler(void) {
    if (EXTI->PR & (1U << LEFT_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR3;

        if (debouncing_buttons[LEFT_BTN_POSITION] == 0) {
            debouncing_buttons[LEFT_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[LEFT_BTN_POSITION]);
        }
    }
}

void EXTI4_IRQHandler(void) {
    if (EXTI->PR & (1U << RIGHT_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR4;
        
        if (debouncing_buttons[RIGHT_BTN_POSITION] == 0) {
            debouncing_buttons[RIGHT_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[RIGHT_BTN_POSITION]);
        }
    }
}

void EXTI9_5_IRQHandler(void) {
    if (EXTI->PR & (1U << UP_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR5;
        
        if (debouncing_buttons[UP_BTN_POSITION] == 0) {
            debouncing_buttons[UP_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[UP_BTN_POSITION]);
        }
    }
    if (EXTI->PR & (1U << DOWN_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR6;

        if (debouncing_buttons[DOWN_BTN_POSITION] == 0) {
            debouncing_buttons[DOWN_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[DOWN_BTN_POSITION]); 
        }
    }
}

void EXTI0_IRQHandler(void) {
    if (EXTI->PR & (1U << MODE_BTN_PIN)) {
        EXTI->PR = EXTI_PR_PR0;
        
        if (debouncing_buttons[MODE_BTN_POSITION] == 0) {
            debouncing_buttons[MODE_BTN_POSITION] = DEBOUCING_TICKS;
            handle_button(&buttons[MODE_BTN_POSITION]);
        }
    }
}

/**
 * Timer interrutpion handler.
 * It resets the button block time after 8 clock cycles (as DEBOUNCING_TICKS is 0xFF).
 */
void TIM3_IRQHandler(void) {
    uint32_t it_status = TIM3->SR & TIM3->DIER;

    if (it_status & TIM_SR_UIF) {
        TIM3->SR = ~it_status;

        for (int i = 0; i < BUTTONS_COUNT; i++) {
            if (debouncing_buttons[i] > 0) {
            	debouncing_buttons[i]--;
            }
        }
    }    
}

int main(void) {
    // Set buffers
    memset(dma_buf, '\0', BUFF_SIZE);
    dma_buf_len = 0;

    memset(buffered_messages, '\0', BUFF_SIZE*sizeof(char*));
    buffered_count = 0;

    // Initial system configuration (GPIOs, DMA{1,2}, USART{1,2}, TIM3).
    configure_system();

    // Turn off LEDs and configure theirs GPIOs.
    setup_leds();

    // TIM3 configuration to eliminate button bouncing.
    configure_timer();

    // USART configuration (GPIOA pins 9/10 for USART1, GPIOA pins 2/3 for USART2).
    configure_uart();
    
    // Configure DMA{1,2} for USART{2,1} Tx and Rx and enable DMA interruptions.
    configure_dma();

    // Configure buttons and enable their (external) interruptions.
    setup_buttons();

    // At the end enable UART and Timer (with timer's interruption).
    enable_uart();
    enable_timer();

    // Initializes first DMA receiving transfer, otherwise no data would be transferred.
    initialize_user_input();

    // We don't wait actively for events and print,
    // instead we implement the interruptions handling
    // and setup all neccessary things for buttons 
    // to cause interruptions and print changes via DMA
    while (true);

    return 0;
}
