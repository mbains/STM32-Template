#include "hx711.h"
#include "stdint.h"

typedef struct {
    EZGPIO_Interface * _sck;
    EZGPIO_Interface * _dout;
    int8_t _gain;
    int32_t _offset;
    uint32_t _scale;
} hx_iface_t;

static hx_iface_t m_hx_iface;

int8_t hx711_init(EZGPIO_Interface * sck, EZGPIO_Interface *dout) {
    m_hx_iface._sck = sck;
    m_hx_iface._dout = dout;
    m_hx_iface._gain = 1;
    return 0;
}
    
int8_t hx711_is_ready() {
    return EZGPIO_ReadInput(m_hx_iface._dout) == 0;
}

int32_t hx711_read() {
    while(!hx711_is_ready());
    
    int32_t result = 0;
    uint8_t data[3];
    uint8_t j;
    uint8_t i;
    
    for(j = 3; j--;) {
        for(i = 8; i--;) {
            EZGPIO_SetOutput(m_hx_iface._sck, 1);
            data[j] |= (EZGPIO_ReadInput(m_hx_iface._dout) << i);
            EZGPIO_SetOutput(m_hx_iface._sck, 0);
        }
    }
    
    for (i = 0; i < m_hx_iface._gain; i++) {
        EZGPIO_SetOutput(m_hx_iface._sck, 1);
        EZGPIO_SetOutput(m_hx_iface._sck, 0);
        
    }
    data[2] ^= 0x80;
    
    result = ((uint32_t) data[2] << 16) | ((uint32_t) data[1] << 8) | (uint32_t) data[0];
    result = result/1000;
    return result;
    
}

int32_t hx711_read_average(uint8_t times) {
    int32_t read_val;
    int32_t sum = 0;
    uint8_t i = 0;
    for(; i < times; i++) {
        sum += hx711_read();
        //iprintf("sum = %d, interval = %d, div=%d\r\n", sum, i+1, (sum/ (i+1)));
    }
    return sum/times;
}

int32_t hx711_get_value(uint8_t times) {
    return (hx711_read_average(times) - m_hx_iface._offset);
}

int32_t hx711_get_single_value() {
    return (hx711_read() - m_hx_iface._offset);
}

double hx711_get_units(uint8_t times) {
    return hx711_get_value(times) / m_hx_iface._scale;
}

void hx711_tare(uint8_t times) {
    int32_t sum = hx711_read_average(times);
    hx711_set_offset(sum);
}

void hx711_set_offset(int32_t offset) {
    m_hx_iface._offset = offset;
}

void hx711_set_tare(int32_t tare) {
    m_hx_iface._offset = tare;
}