#include "gpio.h"

static EZGPIO_Interface GPIO_userBtn = {GPIOA, RCC_APB2Periph_GPIOA, GPIO_Pin_0};
static EZGPIO_Interface GPIO_userLed = {GPIOC, RCC_APB2Periph_GPIOC, GPIO_Pin_9};

inline static void GPIO_Configure(EZGPIO_Interface * iface_p, GPIOMode_TypeDef mode) {
    GPIO_InitTypeDef cs_struct;  
    
    GPIO_StructInit(&cs_struct);
    RCC_APB2PeriphClockCmd(iface_p->rcc_apb_x, ENABLE);
    
    cs_struct.GPIO_Mode = mode;
    cs_struct.GPIO_Pin = iface_p->pin;
    cs_struct.GPIO_Speed = GPIO_Speed_50MHz; 
    
    GPIO_Init(iface_p->gpio_port, &cs_struct);
}


void EZGPIO_SetOutPP(EZGPIO_Interface * iface_p)
{
    GPIO_Configure(iface_p, GPIO_Mode_Out_PP);
}

void EZGPIO_SetInFloating(EZGPIO_Interface * iface_p)
{
    GPIO_Configure(iface_p, GPIO_Mode_IN_FLOATING);
}

void EZGPIO_SetInPullUp(EZGPIO_Interface* iface_p) {
     GPIO_Configure(iface_p, GPIO_Mode_IPU);
}


uint8_t EZGPIO_ReadInput(EZGPIO_Interface * iface_p) {
    return GPIO_ReadInputDataBit(iface_p->gpio_port, iface_p->pin);
}

void EZGPIO_SetOutput(EZGPIO_Interface* iface_p, uint8_t value) {
    GPIO_WriteBit(iface_p->gpio_port, iface_p->pin, value);
}

inline void EZGPIO_InitUserBtn() {
    EZGPIO_SetInFloating(&GPIO_userBtn);
}
inline void EZGPIO_InitUserLed() {
    EZGPIO_SetOutPP(&GPIO_userLed);
}

inline EZGPIO_Interface * EZGPIO_getUserBtn() {
    return &GPIO_userBtn;
}
inline EZGPIO_Interface * EZGPIO_getUserLed() {
    return &GPIO_userLed;
}

