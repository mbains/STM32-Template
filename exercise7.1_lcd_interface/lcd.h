/* 
 * File:   lcd.h
 * Author: mannyb
 *
 * Created on August 17, 2013, 2:06 PM
 */

#ifndef LCD_H
#define	LCD_H
#include "spi.h"

#define MADCTLGRAPHICS  0x6
#define MADCTLBMP       0x2
#define ST7735_width    128
#define ST7735_height   160

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0,
        uint16_t x1, uint16_t y1, uint8_t madctl);

void ST7735_pushColor(uint16_t *color, int cnt);
void ST7735_init();
void ST7735_backLight(uint8_t on);
void ST7735_DrawChar(short x, short y, unsigned char c, short textColor, short bgColor, unsigned char size);
void ST7735_PrintLog(short x, short y, unsigned char * str, uint16_t color);

void fillScreen(uint16_t color);


#endif	/* LCD_H */

