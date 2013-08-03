/* 
 * Exercise 4.2 Blinking Lights with Pushbutton
 * Modify the blinking lights program to additionally track the state of
 * the user pushbutton (PA0) on the blue LED (PC8). See if you can ﬁgure out
 * how to conﬁgure both LEDs with a single call to GPIO_Init.
 */


#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>

void Delay(uint32_t nTime);


int main(void){
    GPIO_InitTypeDef LEDInitStruct;
    GPIO_InitTypeDef BtnStruct;
    
    //(1) enable peripher clock
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
    
    
    //(2) configure pins
    GPIO_StructInit(&LEDInitStruct);
    LEDInitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_8;
    LEDInitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    LEDInitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &LEDInitStruct);
    
    GPIO_StructInit(&BtnStruct);
    BtnStruct.GPIO_Pin = GPIO_Pin_0;
    BtnStruct.GPIO_Mode = GPIO_Mode_IPD;
    BtnStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &BtnStruct);
    
    /*(3) Conigure sys timer*/
    if(SysTick_Config(SystemCoreClock / 1000))
        while(1);
    
    while(1) {
        //(4)
        static int ledval = 0;
        static int btnVal = 0;
        
        /*(4) toggle LED*/
        GPIO_WriteBit(GPIOC, GPIO_Pin_9, (ledval) ? Bit_SET: Bit_RESET);
        ledval = 1 - ledval;
        
        btnVal = GPIO_ReadInputData(GPIOA) & GPIO_Pin_0;
        GPIO_WriteBit(GPIOC, GPIO_Pin_8, btnVal);
        
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

