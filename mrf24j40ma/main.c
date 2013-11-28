
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "usart/usart.h"
#include "gpio/gpio.h"
#include <stdio.h>



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

int main(void){
    int charin;
    EZGPIO_Interface * userBtn = EZGPIO_getUserLed();
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);
    usart_init();
    EZGPIO_InitUserBtn();
    EZGPIO_InitUserLed();
    
    while(1) {
        //Delay(250);
        charin = usart_getc();
        printf("stm32: %c\r\n", charin);
        switch(charin) {
            case 'a':
                EZGPIO_SetOutput(userBtn, 1);
                break;
            default:
                EZGPIO_SetOutput(userBtn, 0);
                
        }
        
    }
    return 0;
}
