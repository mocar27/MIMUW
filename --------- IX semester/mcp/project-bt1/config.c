/**
 * @file config.c
 * @brief Implementation of Configuration functions for Project BT1 - Microcontrollers programming.
 */

#include <irq.h>

#include "config.h"

 /**
  *  Initial system configuration fuction (GPIOs, DMA{1,2}, USART{1,2}, TIM3).
  */
#if defined(USE_UART2)
void configure_system(void) {
    // Setup right GPIOs and set designated DMA.
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA1EN;

    // Setup timer to eliminate button bouncing and configure USART2
    // Use the same TIMx as presented in slides - TIM3 connected to APB1.
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN | RCC_APB1ENR_USART2EN;

    // Configure outside interruptions.
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}
#elif defined(USE_UART1)
void configure_system(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN | RCC_APB2ENR_USART1EN;
}
#endif

/**
 *  UART configuration function (GPIOA pins 2/3 for USART2 and  9/10 for USART1).
 */
void configure_uart(void) {
    // Tx pin is 2 or 9, Rx pin is 3 or 10 (depending on USART_VERSION)
    GPIOafConfigure(GPIOA, USART_TX_PIN, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_NOPULL, USART_GPIO);
    GPIOafConfigure(GPIOA, USART_RX_PIN, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_UP, USART_GPIO);

    // Configure registers, as presented
    USART_VERSION->CR1 = USART_Mode_Rx_Tx | USART_WordLength_8b | USART_Parity_No;
    USART_VERSION->CR2 = USART_StopBits_1;
    USART_VERSION->CR3 = USART_CR3_DMAT | USART_CR3_DMAR; // setup Tx and Rx in DMA
    USART_VERSION->BRR = (PCLK1_HZ + (BAUD / 2U)) / BAUD;
}

/**
 *  Enable UART function.
 */
void enable_uart(void) {
    // Finally set UE in CR1.
    USART_VERSION->CR1 |= USART_Enable;
}

/**
 *  DMA configuration function for USART1 Tx and Rx + enable DMA interruptions.
 */
void configure_dma(void) {
    // USART1 Rx (Receiving stream on DMA2 - stream no. 5, chan. 4)
	DMA_RECV_STREAM->CR = 4U << 25 | DMA_SxCR_PL_1 | DMA_SxCR_MINC | DMA_SxCR_TCIE;
	DMA_RECV_STREAM->PAR = (uint32_t)&USART_VERSION->DR;
    
    // USART1 Tx (Sending stream on DMA2 - stream no. 7, chan. 4)
	DMA_SEND_STREAM->CR = 4U << 25 | DMA_SxCR_PL_1 | DMA_SxCR_MINC | DMA_SxCR_DIR_0 | DMA_SxCR_TCIE;
	DMA_SEND_STREAM->PAR = (uint32_t)&USART_VERSION->DR;

    // Clear markers and setup DMA interruptions on both channels
    DMA_VERSION->HIFCR = DMA_SEND_FLAG;

#if defined(USE_UART2)
    DMA1->HIFCR = DMA_HIFCR_CTCIF5;
#elif defined(USE_UART1)
    DMA2->LIFCR = DMA_LIFCR_CTCIF2;
#endif

    NVIC_EnableIRQ(DMA_RECV_INTERRUPTION);
    NVIC_EnableIRQ(DMA_SEND_INTERRUPTION);
}

/**
 * TIM3 configuration function to eliminate button bouncing.
 * Remember that, as now we're using TIMx to debounce buttons,
 * sometimes spamming the buttons might cause microcontroller to not print that the button 
 * was released. Considerable then would be increasing the frequency of debouncing 
 * or lowering the number of DEBOUCING_TICKS
 */
void configure_timer(void) {    
    // GPIOafConfigure(GPIOA, 6, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL, GPIO_AF_TIM3);
    // GPIOafConfigure(GPIOA, 7, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL, GPIO_AF_TIM3);

    TIM3->PSC = 1599;            // Frequency = 16Mhz / (PSC + 1) * (ARR + 1)
    TIM3->ARR = 49;
    TIM3->CR1 = TIM_CR1_URS;     // Only counter overflow generates TIM3 interruption.
    TIM3->EGR = TIM_EGR_UG;      // Update Generation (przeładowanie PSC/ARR)

    TIM3->SR = ~TIM_SR_UIF;      // Clear SR flag to not enter interruption at the beginning.
    TIM3->DIER = TIM_DIER_UIE;
}

/**
 *  Enable Timer function + enable timer interruptions.
 */
void enable_timer(void) {
    // Enable TIM3 interrupts and start the timer
    NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;
}
