#ifndef CONFIG_H
#define CONFIG_H

#include <gpio.h>

// Define configuration during compilation.
#if defined(USE_UART2)
    #define USART_VERSION USART2
    #define USART_GPIO GPIO_AF_USART2
    #define USART_TX_PIN 2
    #define USART_RX_PIN 3
    #define DMA_SEND_STREAM DMA1_Stream6
    #define DMA_RECV_STREAM DMA1_Stream5
    #define DMA_SEND_FLAG DMA_HISR_TCIF6
    #define DMA_RECV_FLAG DMA_HISR_TCIF5
    #define DMA_VERSION DMA1
    #define DMA_SEND_INTERRUPTION DMA1_Stream6_IRQn
    #define DMA_RECV_INTERRUPTION DMA1_Stream5_IRQn
#elif defined(USE_UART1)
    #define USART_VERSION USART1
    #define USART_GPIO GPIO_AF_USART1
    #define USART_TX_PIN 9
    #define USART_RX_PIN 10
    #define DMA_SEND_STREAM DMA2_Stream7
    #define DMA_RECV_STREAM DMA2_Stream2
    #define DMA_SEND_FLAG DMA_HISR_TCIF7
    #define DMA_RECV_FLAG DMA_HISR_TCIF2
    #define DMA_VERSION DMA2
    #define DMA_SEND_INTERRUPTION DMA2_Stream7_IRQn
    #define DMA_RECV_INTERRUPTION DMA2_Stream2_IRQn
#endif

// CR1
#define USART_Mode_Rx_Tx (USART_CR1_RE | USART_CR1_TE)
#define USART_Enable USART_CR1_UE
// CR1 WordLen
#define USART_WordLength_8b 0x0000
// CR1 parity
#define USART_Parity_No 0x0000

// CR2 stop bits
#define USART_StopBits_1 0x0000

// BRR 
#define PCLK1_HZ 16000000U
#define BAUD 9600U

#define DEBOUCING_TICKS 8

// Config functions declarations
void configure_system(void);
void configure_uart(void);
void enable_uart(void);
void configure_dma(void);
void configure_timer(void);
void enable_timer(void);

#endif // CONFIG_H
