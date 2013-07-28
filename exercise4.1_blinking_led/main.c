#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

void Delay(uint32_t nTime);


int main(void){
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //(1) enable peripher clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    //(2) configure pins
    GPIO_StructInit(&GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    /*(3) Conigure sys timer*/
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);
    
    while(1) {
        //(4)
        static int ledval = 0;
        
        /*(4) toggle LED*/
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledval) ? Bit_SET: Bit_RESET);
        ledval = 1 - ledval;
        
        Delay(250);
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

