#include "lcd.h"

#define LOW             0
#define HIGH            1

#define LCD_C           LOW
#define LCD_D           HIGH

#define LCD_PORT        GPIOC
#define GPIO_PIN_SCE    GPIO_Pin_0
#define GPIO_PIN_RST    GPIO_Pin_1
#define GPIO_PIN_DC     GPIO_Pin_2
#define GPIO_PIN_BKL    GPIO_Pin_1
#define SPILCD          SPI2
#define LCDSPEED        SPI_SLOW

#define ST7735_CASET    0x2A
#define ST7735_RASET    0x2B
#define ST7735_MADCTL   0x36
#define ST7735_RAMWR    0x2C
#define ST7735_RAMRD    0x2E
#define ST7735_COLMOD   0x3A


#define ST7735_SWRESET          0x01
#define ST7735_SLPOUT           0x11
#define ST7735_FRMCTR1          0xB1
#define ST7735_FRMCTR2          0xB2
#define ST7735_FRMCTR3          0xB3
#define ST7735_INVCTR           0xB4
#define ST7735_PWCTR1           0xC0
#define ST7735_PWCTR2           0xC1
#define ST7735_PWCTR3           0xC2
#define ST7735_PWCTR4           0xC3
#define ST7735_PWCTR5           0xC4
#define ST7735_VMCTR1           0xC5
#define ST7735_INVOFF           0x20
#define ST7735_GMCTRP1          0xE0
#define ST7735_GMCTRP2          0xE1
#define ST7735_DISPON           0x29
#define ST7735_NORON            0x13


#define MADVAL(x) (((x) << 5) | 8)

static uint8_t madctlcurrent = MADVAL(MADCTLGRAPHICS);

struct ST7735_cmdBuf {
    uint8_t command;
    uint8_t delay;
    uint8_t len;
    uint8_t data[16];
};

static const struct ST7735_cmdBuf initializers[] = {
    
    {   ST7735_SWRESET, 150, 0, 0 },
    {   ST7735_SLPOUT,  150, 0, 0 },
    {   ST7735_FRMCTR1, 0,   3, {0x01, 0x2c, 0x2d} },
    {   ST7735_FRMCTR2, 0,   3, {0x01, 0x2c, 0x2d} },
    {   ST7735_FRMCTR3, 0,   6, {0x01, 0x2c, 0x2d, 0x01, 0x2c, 0x2d} },
    {   ST7735_INVCTR,  0,   1, {0x07} },
    {   ST7735_PWCTR1,  0,   3, {0xa2, 0x02, 0x84}},
    {   ST7735_PWCTR2,  0,   1, {0xc5}},
    {   ST7735_PWCTR3,  0,   2, {0xa, 0x0}},
    {   ST7735_PWCTR4,  0,   2, {0x8a, 0x2a}},
    {   ST7735_PWCTR5,  0,   2, {0x8a, 0xee}},
    {   ST7735_VMCTR1,  0,   1, {0x0e}},
    {   ST7735_INVOFF,  0,   0, {0}},
    {   ST7735_MADCTL,  0,   1, {MADVAL(MADCTLGRAPHICS) }},
    {   ST7735_COLMOD,  0,   1, {0x05}},
    {   ST7735_CASET,   0,   4, {0x00, 0x00, 0x00, 0x7F}},
    {   ST7735_RASET,   0,   4, {0x00, 0x00, 0x00, 0x9F}},
    {   ST7735_GMCTRP1, 0,  16, {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29,
            0x2d, 0x29, 0x25, 0x2b, 0x39, 0x00, 0x01, 0x03, 0x10}},
    
    {   ST7735_GMCTRP2, 0,  16, {0x03, 0x1d, 0x07, 0x06, 0x02e, 0x2c, 0x29, 
            0x2D, 0x2e, 0x2e, 0x37, 0x3f, 0x00, 0x00, 0x02, 0x10}},
            
    {   ST7735_DISPON,  100, 0, {0}},
    {   ST7735_NORON,   10,  0, {0}},
    {   0,              0,   0, {0}}
    
};

static void LcdWrite(char dc, const char * data, int nbytes);
static void LcdWrite16(char dc, const uint16_t * data, int cnt);
static void Enable_GPIO(uint32_t rcc_apb_x, GPIO_TypeDef * gpio_x, uint16_t pin);

static void Enable_GPIO(uint32_t rcc_apb_x, GPIO_TypeDef * gpio_x, uint16_t pin)
{
    GPIO_InitTypeDef cs_struct;  
    
    GPIO_StructInit(&cs_struct);
    RCC_APB2PeriphClockCmd(rcc_apb_x, ENABLE);
    
    cs_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    cs_struct.GPIO_Pin = pin;
    cs_struct.GPIO_Speed = GPIO_Speed_50MHz; //2 MHz suffice?
    
    GPIO_Init(gpio_x, &cs_struct);
}




void ST7735_init() {
    
    const struct ST7735_cmdBuf * cmd;
    //misc GPIO to drive LCD
    Enable_GPIO(RCC_APB2Periph_GPIOC, LCD_PORT, GPIO_PIN_SCE); //LCD SPI CS, PC0 on STM32
    Enable_GPIO(RCC_APB2Periph_GPIOC, LCD_PORT, GPIO_PIN_RST); //LCD Reset, PC1 on STM32
    Enable_GPIO(RCC_APB2Periph_GPIOC, LCD_PORT, GPIO_PIN_DC); //LCD Data/Control(RS) switch, PC2 on STM32
    Enable_GPIO(RCC_APB2Periph_GPIOA, GPIOA,    GPIO_PIN_BKL); //LCD backlight ctrl, PA1 on STM32
    
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_SCE, HIGH);
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, HIGH);
    Delay(10);
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, LOW);
    Delay(10);
    GPIO_WriteBit(LCD_PORT, GPIO_PIN_RST, HIGH);
    Delay(10);
    
    for(cmd = initializers; cmd->command; cmd++) 
    {
        LcdWrite(LCD_C, &(cmd->command), 1);
        if(cmd->len)
            LcdWrite(LCD_D, cmd->data, cmd->len);
        if(cmd->delay)
            Delay(cmd->delay);
    }
    
    
}

void fillScreen(uint16_t color)
{
    uint8_t x, y;
    ST7735_setAddrWindow(0, 0, ST7735_width-1, ST7735_height-1, MADCTLGRAPHICS);
    
    for(x = 0; x < ST7735_width; x++) {
        for(y=0; y < ST7735_height; y++) {
            ST7735_pushColor(&color, 1);
        }
    }
}

static void LcdWrite(char dc, const char * data, int nbytes)
 {
     GPIO_WriteBit(LCD_PORT, GPIO_PIN_DC, dc); //dc; 1 = data, 0 = control
     
     GPIO_ResetBits(LCD_PORT, GPIO_PIN_SCE);
     spiReadWrite(SPILCD, 0, data, nbytes, LCDSPEED);
     GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
 }
 
static void LcdWrite16(char dc, const uint16_t * data, int cnt)
 {
     GPIO_WriteBit(LCD_PORT, GPIO_PIN_DC, dc); //dc; 1 = data, 0 = control
     
     GPIO_ResetBits(LCD_PORT, GPIO_PIN_SCE);
     spiReadWrite16(SPILCD, 0, data, cnt, LCDSPEED);
     GPIO_SetBits(LCD_PORT, GPIO_PIN_SCE);
 }
  
static void ST7735_writeCmd(uint8_t c)
{
    LcdWrite(LCD_C, &c, 1);
}

void ST7735_pushColor(uint16_t* color, int cnt)
{
    LcdWrite16(LCD_D, color, cnt);
}

void ST7735_setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t madctl)
{
    madctl = MADVAL(madctl);
    if(madctl != madctlcurrent) {
        ST7735_writeCmd(ST7735_MADCTL);
        LcdWrite(LCD_D, &madctl, 1);
        madctlcurrent = madctl;
    }
    
    ST7735_writeCmd(ST7735_CASET);
    LcdWrite16(LCD_D, &x0, 1);
    LcdWrite16(LCD_D, &x1, 1);
    
    
    ST7735_writeCmd(ST7735_RASET);
    LcdWrite16(LCD_D, &y0, 1);
    LcdWrite16(LCD_D, &y1, 1);
    
    ST7735_writeCmd(ST7735_RAMWR);
}
