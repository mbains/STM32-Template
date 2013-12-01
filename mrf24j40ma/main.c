
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "usart/usart.h"
#include "gpio/gpio.h"
#include "mrf24j40.h"



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
    
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);
    
   // char * heap_end;
    int charin;
    EZGPIO_Interface * userLED = EZGPIO_getUserLed();
    EZGPIO_Interface * userBtn = EZGPIO_getUserBtn();

    EZGPIO_Interface cs = {.gpio_port = GPIOC, .pin = GPIO_Pin_0, .rcc_apb_x = RCC_APB2Periph_GPIOC};
    EZGPIO_Interface reset = {.gpio_port = GPIOC, .pin = GPIO_Pin_1, .rcc_apb_x = RCC_APB2Periph_GPIOC};

    EZGPIO_SetOutPP(&cs);
    EZGPIO_SetOutPP(&reset);
    
    usart_init();
    mrf24j40_init(&cs, &reset);
    EZGPIO_InitUserBtn();
    EZGPIO_InitUserLed();

    while(1) {
        //Delay(250);
        //charin = usart_getc();
        Delay(600);
        iprintf("stm32: %c\r\n", charin);
        //iprintf("ack = %x \r\n",mrf24j40_getAckTMOut());
        mrf24j40_devinit();
        switch(charin) {
            case 'a':
                EZGPIO_SetOutput(userLED, EZGPIO_ReadInput(userBtn));
                break;
            default:
                EZGPIO_SetOutput(userLED, 0);
                
        }
        
    }
    return 0;
}
