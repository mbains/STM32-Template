
#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include "usart/usart.h"
#include "gpio/gpio.h"
#include "mrf24j40.h"

#define MRF_BOARD_B

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
#ifdef MRF_BOARD_B
    myaddr = 0x6000;
    destaddr = 0x6001;
#else
    myaddr = 0x6001;
    destaddr = 0x6000;
#endif
   // char * heap_end;
    int evt_count = 0;
    mrf_evt evt;
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

    mrf24j40_devinit();
    Delay(100);
    mrf24j40_setpan(0xcafe);
//   This is _our_ address
    mrf24j40_setShortAddr(myaddr);
    while(1) {

        Delay(600);
        mrf24j40_interrupt_handler();
        evt = mrf24j40_check_flags();
        if(evt & mrf_rxevent) {
            iprintf("mrf_rxevent\r\n");
            EZGPIO_SetOutput(userLED, (evt_count++%2));
        }
        if(evt & mrf_txevent) {
            iprintf("mrf_txevent\r\n");
        }
        mrf24j40_send16(destaddr, "hello");
        
    }
    return 0;
}
