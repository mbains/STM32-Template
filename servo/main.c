#include <stm32f10x.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_tim.h>

#include "gpio/gpio.h"

void Delay(uint32_t nTime);

int main(void) {
    if (SysTick_Config(SystemCoreClock / 1000)) {
        while (1);
    }
        
    
    //must enable RCC_AFIO for bus 2 for PWM
    EZGPIO_Interface tim_GPIO = {GPIOA, RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, GPIO_Pin_1};
    
    EZGPIO_SetAFPP(&tim_GPIO);
    
    //EZGPIO_SetOutput(&tim_GPIO, 1);

    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_OCInitTypeDef TIM_OCInitStructure;

    // enable timer clock

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // configure timer

    // PWM frequency = 50 hz with 24,000,000 hz system clock for a 20ms period

    // 24,000,000/240 = 100,000

    // 100,000/2000 = 50

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);

    TIM_TimeBaseStructure.TIM_Prescaler

            = SystemCoreClock / 100000 - 1; // 0..239

    TIM_TimeBaseStructure.TIM_Period = 2000 - 1; // 0..999

    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // PWM1 Mode configuration: Channel2

    // Edge-aligned; not single pulse mode

    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;

    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

    //TIM_OC2Init is for channel 2
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    // Enable Timer

    TIM_Cmd(TIM2, ENABLE);
    
   

    while (1) {
        int interval = 70;
        for(; interval < 220; interval+=1) {
            TIM_SetCompare2(TIM2, interval);
            Delay(40);
        }
        for(; interval > 70; interval+= -1) {
            TIM_SetCompare2(TIM2, interval);
            Delay(40);
        }
    }
}

/*(5) Timer code*/
static __IO uint32_t TimingDelay;

void Delay(uint32_t nTime) {
    TimingDelay = nTime;
    while (TimingDelay != 0);
}

void SysTick_Handler(void) {
    if (TimingDelay != 0x00)
        TimingDelay--;
}

#ifdef USE_FULL_ASSERT 

void assert_failed(uint8_t * file, uint32_t line) {
    /* Infinite loop*/
    /* use GDB to find where we're here*/
    while (1);
}
#endif

