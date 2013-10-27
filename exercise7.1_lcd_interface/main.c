#include <stm32f10x.h>
#include "spi.h"
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

static uint8_t txbuf[4], rxbuf[4];
static uint16_t txbuf16[4], rxbuf16[4];

void Delay(uint32_t nTime);



int main(void){
    
    spiInit(SPI2);
    
    
    
    return 0;
}

int spiTest() 
{
    int i,j;
    int total8 = 0;
    int total16 = 0;
    
    spiInit(SPI2);
    for(i = 0; i < 8; i++)
    {
        for(j = 0; j< 4; j++)
            txbuf[j] = i*4 + j;
        
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
        spiReadWrite(SPI2, rxbuf, txbuf, 4, SPI_SLOW);
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
        
        for(j = 0; j < 4; j++)
            if(rxbuf[j]  != txbuf[j])
                assert_failed(__FILE__, __LINE__);
            else
                total8++;
            
    }
    
    for(i = 0; i < 8; i++){
        for(j = 0; j < 4; j++) 
            txbuf16[j] = i*4 + j + (i<<8);
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, 0);
        spiReadWrite16(SPI2, rxbuf16, txbuf16, 4, SPI_SLOW);
        GPIO_WriteBit(GPIOC, GPIO_Pin_3, 1);
        for(j = 0; j < 4; j++)
            if(rxbuf16[j]  != txbuf16[j])
                assert_failed(__FILE__, __LINE__);
            else
                total16++;
    }
    
    return 0;
}
/*(5) Timer code*/
static __IO uint32_t TimingDelay;
void Delay(uint32_t nTime) 
{
    TimingDelay = nTime;
    while(TimingDelay !=0);
}

void SysTick_Handler(void) 
{
    if(TimingDelay != 0x00) 
        TimingDelay--;
}

#ifdef USE_FULL_ASSERT 
void assert_failed(uint8_t * file, uint32_t line)
{
    /* Infinite loop*/
    /* use GDB to find where we're here*/
    while(1);
}
#endif

