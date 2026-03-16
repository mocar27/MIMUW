#include <stm32.h>
#include <gpio.h>
#include <string.h>

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
#define RED_LED_GPIO    GPIOA
#define GREEN_LED_GPIO  GPIOA
#define BLUE_LED_GPIO   GPIOB
#define GREEN2_LED_GPIO GPIOA

#define USER_BTN_GPIO  GPIOC
#define LEFT_BTN_GPIO  GPIOB
#define RIGHT_BTN_GPIO GPIOB
#define UP_BTN_GPIO    GPIOB
#define DOWN_BTN_GPIO  GPIOB
#define FIRE_BTN_GPIO  GPIOB
#define MODE_BTN_GPIO  GPIOA

// Define Outs
#define RED_LED_PIN    6
#define GREEN_LED_PIN  7
#define BLUE_LED_PIN   0
#define GREEN2_LED_PIN 5

#define USER_BTN_PIN  13
#define LEFT_BTN_PIN  3
#define RIGHT_BTN_PIN 4
#define UP_BTN_PIN    5
#define DOWN_BTN_PIN  6
#define FIRE_BTN_PIN  10
#define MODE_BTN_PIN  0

// Leds control defs
#define RED_LED    'R'
#define GREEN_LED  'G'
#define BLUE_LED   'B'
#define GREEN2_LED 'g'

#define LED_INDICATOR 'L'
#define LED_ON        '1'
#define LED_OFF       '0'
#define LED_TOGGLE    'T'


// Global structs for Recv and Send
// Recv communicates on micro (changing lights of leds depending on the user input)
char recv_buf[3];
int received = 0;

// Sending communicates from micro to minicom (left, right, up, down, fire, user, mode)
typedef struct Cyclic_buf {
	char buffer[1024];
	unsigned head;
    unsigned tail;
	unsigned size_of_output;
} Cyclic_buf;
Cyclic_buf send_buf;

typedef struct Button {
    GPIO_TypeDef* gpio;
    uint8_t pin;
    char* name;
    uint8_t last_state;
} Button;
Button buttons[] = {
    {USER_BTN_GPIO, USER_BTN_PIN, "USER", 1},
    {LEFT_BTN_GPIO, LEFT_BTN_PIN, "LEFT", 1},
    {RIGHT_BTN_GPIO, RIGHT_BTN_PIN, "RIGHT", 1},
    {UP_BTN_GPIO, UP_BTN_PIN, "UP", 1},
    {DOWN_BTN_GPIO, DOWN_BTN_PIN, "DOWN", 1},
    {FIRE_BTN_GPIO, FIRE_BTN_PIN, "FIRE", 1},
    {MODE_BTN_GPIO, MODE_BTN_PIN, "MODE", 0}
};
#define BUTTONS_COUNT (sizeof(buttons) / sizeof(buttons[0]))

// Keep number of last toggled led to toggle right led on/off
// Then when doing off/toggle, we check whether correct LED is on and we do it
// Otherwise, we ignroe the command
char toggled_led = ' ';

#define RedLEDon() RED_LED_GPIO->BSRR = 1 << (RED_LED_PIN + 16)
#define RedLEDoff() RED_LED_GPIO->BSRR = 1 << RED_LED_PIN

#define GreenLEDon() GREEN_LED_GPIO->BSRR = 1 << (GREEN_LED_PIN + 16)
#define GreenLEDoff() GREEN_LED_GPIO->BSRR = 1 << GREEN_LED_PIN

#define BlueLEDon() BLUE_LED_GPIO->BSRR = 1 << (BLUE_LED_PIN + 16)
#define BlueLEDoff() BLUE_LED_GPIO->BSRR = 1 << BLUE_LED_PIN

#define Green2LEDon() GREEN2_LED_GPIO->BSRR = 1 << GREEN2_LED_PIN
#define Green2LEDoff() GREEN2_LED_GPIO->BSRR = 1 << (GREEN2_LED_PIN + 16)

static void switch_led_on(char led_color_char) {
    switch (led_color_char) {
        case RED_LED:
            RedLEDon();
            break;
        case GREEN_LED:
            GreenLEDon();
            break;
        case BLUE_LED:
            BlueLEDon();
            break;
        case GREEN2_LED:
            Green2LEDon();
            break;
        default:
            break;
    }
}

static void switch_led_off(char led_color_char) {
    switch (led_color_char) {
        case RED_LED:
            RedLEDoff();
            break;
        case GREEN_LED:
            GreenLEDoff();
            break;
        case BLUE_LED:
            BlueLEDoff();
            break;
        case GREEN2_LED:
            Green2LEDoff();
            break;
        default:
            break;
    }
}

static void handle_led_operation(char led_color_char, char operation_char) {
    // Managing LED operations based on received commands from computer terminal at minicom
    // LR1, LR0, LRT;
    // LG1, LG0, LGT;
    // LB1, LB0, LBT;
    // Lg1, Lg0, LgT;
    
    // If we get LED_ON, we just turn on the right LED color
    if (operation_char == LED_ON) {
        switch_led_on(led_color_char);
        toggled_led = led_color_char; // remember last toggled led
        return;
    }
    // If we get LED_OFF, we turn off the right LED color only if it was the last toggled one, othewrise ignore instruction
    else if (operation_char == LED_OFF) {
        switch_led_off(led_color_char);
        return;
    }
    // If we get LED_TOGGLE, we check whether the led is currently on or off based on last toggled led
    else { // LED_TOGGLE
        if (toggled_led != led_color_char) {
            // currently off, so just turn it on and update last toggled led
            switch_led_off(toggled_led);
            switch_led_on(led_color_char);
            toggled_led = led_color_char;
        } else {
            // currently on, so turn just turn it off and reset last toggled led
            switch_led_off(led_color_char);
            toggled_led = ' ';
        }
    }
}

static void receive_user_input() {
    // read char by char, until we reach to receiving 3 chars,
    // if we've received 3 chars correct chars, we parse them and change the led lights

    // if any instruction on the way of constructing the operation is incorrect,
    // we just discard the whole instruction (say that received bytes are 0 again)
    // first char of instruction must be L
    if (received == 1 && recv_buf[0] != 'L') { received = 0; }

    recv_buf[received] = USART2->DR;
    received++;

    // to ensure, possibility of instrction "LLR1" is handled correctly
    // when encountering wrong expected character, we shift the buffer
    if (received == 2) {    // second char must be R/G/B/g
        switch (recv_buf[1]) {
            case 'R':
            case 'G':
            case 'B':
            case 'g':
                break;
            default:
                received = 1; // incorrect second char, discard the instruction
                recv_buf[0] = recv_buf[1];
                return; 
        }
    }
    else if (received == 3) {
        switch(recv_buf[2]) {
            case '0':
            case '1':
            case 'T': 
                handle_led_operation(recv_buf[1], recv_buf[2]);
                received = 0; // after handling, reset received count
            default:
                received = 1; // incorrect third char, discard the instruction
                recv_buf[0] = recv_buf[2];
                return;
        }
    }
}

// Buttons pressing logic
static void save_to_send_buffer(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        send_buf.buffer[send_buf.tail] = str[i];
        send_buf.tail = (send_buf.tail + 1) % sizeof(send_buf.buffer);

        // Prevent buffer overflow, cycle to the beginning
        if (send_buf.size_of_output < sizeof(send_buf.buffer)) {
            send_buf.size_of_output++;
        } 
        else {
            send_buf.head = (send_buf.head + 1) % (sizeof(send_buf.buffer));
        }
    }
}

static void check_button(Button* btn) {
    // Joystick
    // LEFT PRESSED, LEFT RELEASED, 
    // RIGHT PRESSED, RIGH RELEASED, 
    // UP PRESSED, UP RELEASED, 
    // DOWN PRESSED, DOWN RELEASED, 

    // FIRE PRESSED, FIRE RELEASED, 
    // USER PRESSED, USER RELEASED, 
    // MODE PRESSED, MODE RELEASED;

    uint8_t current_state = (btn->gpio->IDR >> btn->pin) & 1 ? 1 : 0;
    // If the state of the button did not change from the last checkup, we do nothing
    if (current_state == btn->last_state) { return; }
    btn->last_state = current_state;
    
    save_to_send_buffer(btn->name);
    save_to_send_buffer(" ");
    
    // Buttons other than MODE are active low (pressed = 0)
    if (strcmp("MODE", btn->name) == 0) { save_to_send_buffer(current_state ? "PRESSED\r\n" : "RELEASED\r\n"); }
    else { save_to_send_buffer(current_state ? "RELEASED\r\n" : "PRESSED\r\n"); }
}

// Initial configuration
static void configure_system(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    send_buf.head = 0;
    send_buf.tail = 0;
    send_buf.size_of_output = 0;
}

static void configure_leds(void) {
    RedLEDoff();
    GreenLEDoff();
    BlueLEDoff();
    Green2LEDoff();

    GPIOoutConfigure(RED_LED_GPIO, RED_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    GPIOoutConfigure(GREEN_LED_GPIO, GREEN_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    GPIOoutConfigure(BLUE_LED_GPIO, BLUE_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
    GPIOoutConfigure(GREEN2_LED_GPIO, GREEN2_LED_PIN, GPIO_OType_PP, GPIO_Low_Speed, GPIO_PuPd_NOPULL);
}

static void configure_uart(void) {
    GPIOafConfigure(GPIOA, 2, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_NOPULL, GPIO_AF_USART2);
    GPIOafConfigure(GPIOA, 3, GPIO_OType_PP, GPIO_Fast_Speed, GPIO_PuPd_UP, GPIO_AF_USART2);

    // Configure registers, as presented
    USART2->CR1 = USART_Mode_Rx_Tx | USART_WordLength_8b | USART_Parity_No;
    USART2->CR2 = USART_StopBits_1;
    USART2->CR3 = USART_FlowControl_None;
    USART2->BRR = (PCLK1_HZ + (BAUD / 2U)) / BAUD;

    // Finally set UE in CR1
    USART2->CR1 |= USART_Enable;
}

int main(void) {
    // Configure right GPIOs and UART
    configure_system();

    // Leds configure
    configure_leds();

    // UART configure
    configure_uart();

    for (;;) {
        // Receiving the char from user, parse it and change the light of led
        // There was a change on the user input in minicom
        if (USART2->SR & USART_SR_RXNE) { receive_user_input(); }

        // We check all buttons for whether there was any change on their state
        for (int i = 0; i < BUTTONS_COUNT; i++) { check_button(&buttons[i]); }
        
        // There is something to send
        if (send_buf.size_of_output > 0) {
            // and we can send it
            if (USART2->SR & USART_SR_TXE) {
                // so just send next char from buffer
                USART2->DR = send_buf.buffer[send_buf.head];
                send_buf.head = (send_buf.head + 1) % sizeof(send_buf.buffer);
                send_buf.size_of_output--;
            }
        }
    }
    return 0;
}
