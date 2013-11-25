#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_usart.h>
#include "ring.h"
#include "usart.h"

static RingBuffer txbuffer = {0, 0};
static RingBuffer rxbuffer = {0, 0};

#define countof(a)   (sizeof(a) / sizeof(*(a)))

uint8_t txbuf[] = "Hello\r\n";
#define txbufsize  (countof(txbuf) - 1)

static int TxPrimed = 0;
int RxOverflow = 0;

int usart_init() {
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 |
    RCC_APB2Periph_AFIO |
    RCC_APB2Periph_GPIOA , ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_StructInit(&GPIO_InitStruct);

    // Initialize USART1_Tx

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;

    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;

    GPIO_Init(GPIOA , &GPIO_InitStruct);

    // Initialize USART1_RX

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA , &GPIO_InitStruct);

    
    // see stm32f10x_usart.h

    USART_InitTypeDef USART_InitStructure;

    // Initialize USART structure

    USART_StructInit(&USART_InitStructure);

    // Modify USART_InitStructure for non-default values , e.g.

    // USART_InitStructure.USART_BaudRate = 38400;

    USART_InitStructure.USART_BaudRate = 9600;

    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART1 ,&USART_InitStructure);

    USART_Cmd(USART1 , ENABLE);
    
    
    /* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART1_IRQn);

    return 0;
}

int usart_TxTest()
{
    int idx;
    for(idx=0; idx < txbufsize; idx++) {
        USART_SendData(USART1, txbuf[idx]);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}

void USART1_IRQHandler(void) {
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        uint8_t data;
        //buffer the data or toss it f there is no room
        //flow control will prevent this
        data = USART_ReceiveData(USART1) & 0xff;
        if(!Enqueue(&rxbuffer, data))
            RxOverflow = 1;
        
    }
    
    if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET) {
        uint8_t data;
        
        if(Dequeue(&txbuffer, &data)){
            USART_SendData(USART1, data);
        } else {
            //if nothing to send disable interrupt
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
            TxPrimed = 0;
        }
    }
}

int getchar(void) 
{       
    uint8_t data;
    while(!Dequeue(&rxbuffer, &data));
    return data;
}

int putchar(int c)
{
    while(!Enqueue(&txbuffer, c)); 
    
    if(!TxPrimed) {
        TxPrimed = 1;
        USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
    }
    
}

int usart_write_arr(char *p, int len)
{
    int i;
    
    for(i=0; i<len; i++) {
        putchar(&txbuffer, *p++);
    }
    return len;
}
