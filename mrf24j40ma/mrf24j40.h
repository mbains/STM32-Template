/* 
 * File:   mrf24j40.h
 * Author: mannyb
 *
 * Created on November 28, 2013, 11:08 AM
 */

#ifndef MRF24J40_H
#define	MRF24J40_H

#include <gpio/gpio.h>
#include <spi/spi.h>

void mrf24j40_init(EZGPIO_Interface * cs, EZGPIO_Interface * reset);
uint8_t mrf24j40_getAckTMOut();
void mrf24j40_devinit();

#endif	/* MRF24J40_H */

