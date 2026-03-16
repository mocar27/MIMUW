CC 		 = arm-eabi-gcc
OBJCOPY  = arm-eabi-objcopy
FLAGS 	 = -mthumb -mcpu=cortex-m4
CPPFLAGS = -DSTM32F411xE
CFLAGS 	 = $(FLAGS) -Wall -g \
		-O2 -ffunction-sections -fdata-sections \
		-I/opt/arm/stm32/inc \
		-I/opt/arm/stm32/CMSIS/Include \
 -I/opt/arm/stm32/CMSIS/Device/ST/STM32F4xx/Include
LDFLAGS  = $(FLAGS) -Wl,--gc-sections -nostartfiles \
		-L/opt/arm/stm32/lds -Tstm32f411re.lds

# Select UART version (1 or 2). Default is 2.
# Usage: make UART=1
UART ?= 2

ifeq ($(UART), 1)
    # USART1 uses DMA2 Stream 2 and 7
    CPPFLAGS += -DUSE_UART1
else
    # USART2 uses DMA1 Stream 5 and 6
    CPPFLAGS += -DUSE_UART2
endif

vpath %.c /opt/arm/stm32/src

OBJECTS = main-bt1.o config.o buttons.o leds.o startup_stm32.o gpio.o
TARGET = main-bt1

.SECONDARY: $(TARGET).elf $(OBJECTS)

all: $(TARGET).bin

%.elf : $(OBJECTS)
		$(CC) $(LDFLAGS) $^ -o $@

%.bin : %.elf
		$(OBJCOPY) $< $@ -O binary

clean :
		rm -f *.bin *.elf *.hex *.d *.o *.bak *~