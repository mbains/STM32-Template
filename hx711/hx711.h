#ifndef HX711_INCLUDE
#define HX711_INCLUDE

#include <stdint.h>
#include <gpio/gpio.h>


int8_t hx711_init(EZGPIO_Interface * sck, EZGPIO_Interface *dout);

int8_t hx711_is_ready();

void hx711_set_gain(uint8_t gain);

int32_t hx711_read();

int32_t hx711_read_average(uint8_t times);

int32_t hx711_get_value(uint8_t times);

double hx711_get_units(uint8_t times);

void hx711_tare(uint8_t times);

void hx711_set_scale(double scale);

void hx711_set_offset(int32_t offset);

void hx711_set_tare(int32_t tare);
int32_t hx711_get_single_value();
#endif

