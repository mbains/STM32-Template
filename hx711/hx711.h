#include <stdint.h>

int8_t hx711_init(uint8_t dout, uint8_t pd_sck, uint8_t gain);

int8_t is_ready();

void set_gain(uint8_t gain);

int32_t read();

int32_t read_average(uint8_t times);

double get_value(uint8_t times);

float get_units(uint8_t times);

void tare(uint8_t times);

void set_scale(float scale);


