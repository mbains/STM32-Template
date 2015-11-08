TEMPLATEROOT = ..

# compilation flags for gdb

CFLAGS  = -O0 -g -I../mrf24j40ma
ASFLAGS = -g 

# object files

OBJS=  $(STARTUP) main.o hx711.o usart/ring.o usart/usart.o usart/syscalls.o spi/spi.o gpio/gpio.o mrf24j40ma/mrf24j40.o
OBJS+= stm32f10x_gpio.o stm32f10x_rcc.o stm32f10x_usart.o stm32f10x_spi.o

# include common make file

include $(TEMPLATEROOT)/Makefile.common


