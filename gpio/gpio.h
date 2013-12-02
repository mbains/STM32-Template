/* 
 * File:   gpio.h
 * Author: mannyb
 *
 * Created on November 27, 2013, 11:14 PM
 */

#ifndef GPIO_H
#define	GPIO_H

#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>

typedef struct {
    GPIO_TypeDef * gpio_port;
    uint32_t rcc_apb_x;
    uint16_t pin;
    
} EZGPIO_Interface;



void EZGPIO_SetOutPP(EZGPIO_Interface * iface_p);
void EZGPIO_SetInFloating(EZGPIO_Interface * iface_p);
void EZGPIO_SetInPullUp(EZGPIO_Interface * iface_p);

uint8_t EZGPIO_ReadInput(EZGPIO_Interface * iface_p);
void EZGPIO_SetOutput(EZGPIO_Interface * iface_p, uint8_t value);

void EZGPIO_InitUserBtn();
void EZGPIO_InitUserLed();

EZGPIO_Interface * EZGPIO_getUserBtn();
EZGPIO_Interface * EZGPIO_getUserLed();

#endif	/* GPIO_H */

