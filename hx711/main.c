
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "usart/usart.h"
#include "gpio/gpio.h"
#include "hx711.h"
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
    
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);
    uint16_t myaddr, destaddr;

   // char * heap_end;
    int evt_count = 0;
    EZGPIO_Interface * userLED = EZGPIO_getUserLed();
    EZGPIO_Interface * userBtn = EZGPIO_getUserBtn();

    EZGPIO_Interface sck = {.gpio_port = GPIOC, .pin = GPIO_Pin_0, .rcc_apb_x = RCC_APB2Periph_GPIOC};
    EZGPIO_Interface dout = {.gpio_port = GPIOC, .pin = GPIO_Pin_1, .rcc_apb_x = RCC_APB2Periph_GPIOC};

    EZGPIO_SetOutPP(&sck);
    EZGPIO_SetInFloating(&dout);
    
    usart_init();

    hx711_init(&sck, &dout);
    EZGPIO_InitUserBtn();
    EZGPIO_InitUserLed();
    iprintf("Prepare Tare\r\n");
    Delay(1000);
    iprintf("Begin Tare\r\n");
    int32_t sum = 0;
    int32_t offset = 0;
    //hx711_tare(10);
    iprintf("Tare Complete\r\n");
    while(1) {

        //Delay(50);
        sum = 0;
        //iprintf("test\r\n");
        EZGPIO_SetOutput(userLED, (evt_count++%2));
        sum += hx711_read();
        
        iprintf("read1: %d\r\n", (sum + offset));
        if(EZGPIO_ReadInput(userBtn) == 1) {
            offset = 0;
            offset = offset - sum;
        }
//        sum += (hx711_read());
//        iprintf("read2: %d\r\n", sum/2);
//        sum += (hx711_read());
//        iprintf("read3: %d\r\n", sum/3);
        
//        sum = hx711_get_value(20);
//        iprintf("read average: %d\r\n\r\n", sum);
        //scanf("%c", &input);
        
    }
    return 0;
}
