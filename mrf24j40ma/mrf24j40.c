#include "mrf24j40.h"

/*
 * Driver based on https://github.com/karlp/Mrf24j40-arduino-library/blob/master/mrf24j.cpp
 */
#define MRF_RXMCR 0x00
#define MRF_PANIDL 0x01
#define MRF_PANIDH 0x02
#define MRF_SADRL 0x03
#define MRF_SADRH 0x04
#define MRF_EADR0 0x05
#define MRF_EADR1 0x06
#define MRF_EADR2 0x07
#define MRF_EADR3 0x08
#define MRF_EADR4 0x09
#define MRF_EADR5 0x0A
#define MRF_EADR6 0x0B
#define MRF_EADR7 0x0C
#define MRF_RXFLUSH 0x0D
//#define MRF_Reserved 0x0E
//#define MRF_Reserved 0x0F
#define MRF_ORDER 0x10
#define MRF_TXMCR 0x11
#define MRF_ACKTMOUT 0x12
#define MRF_ESLOTG1 0x13
#define MRF_SYMTICKL 0x14
#define MRF_SYMTICKH 0x15
#define MRF_PACON0 0x16
#define MRF_PACON1 0x17
#define MRF_PACON2 0x18
//#define MRF_Reserved 0x19
#define MRF_TXBCON0 0x1A

// TXNCON: TRANSMIT NORMAL FIFO CONTROL REGISTER (ADDRESS: 0x1B)
#define MRF_TXNCON      0x1B
#define MRF_TXNTRIG     0
#define MRF_TXNSECEN    1
#define MRF_TXNACKREQ   2
#define MRF_INDIRECT    3
#define MRF_FPSTAT      4

#define MRF_TXG1CON 0x1C
#define MRF_TXG2CON 0x1D
#define MRF_ESLOTG23 0x1E
#define MRF_ESLOTG45 0x1F
#define MRF_ESLOTG67 0x20
#define MRF_TXPEND 0x21
#define MRF_WAKECON 0x22
#define MRF_FRMOFFSET 0x23
// TXSTAT: TX MAC STATUS REGISTER (ADDRESS: 0x24)
#define MRF_TXSTAT 0x24
#define TXNRETRY1       7
#define TXNRETRY0       6
#define CCAFAIL         5
#define TXG2FNT         4
#define TXG1FNT         3
#define TXG2STAT        2
#define TXG1STAT        1
#define TXNSTAT         0

#define MRF_TXBCON1 0x25
#define MRF_GATECLK 0x26
#define MRF_TXTIME 0x27
#define MRF_HSYMTMRL 0x28
#define MRF_HSYMTMRH 0x29
#define MRF_SOFTRST 0x2A
//#define MRF_Reserved 0x2B
#define MRF_SECCON0 0x2C
#define MRF_SECCON1 0x2D
#define MRF_TXSTBL 0x2E
//#define MRF_Reserved 0x2F
#define MRF_RXSR 0x30
#define MRF_INTSTAT 0x31
#define MRF_INTCON 0x32
#define MRF_GPIO 0x33
#define MRF_TRISGPIO 0x34
#define MRF_SLPACK 0x35
#define MRF_RFCTL 0x36
#define MRF_SECCR2 0x37
#define MRF_BBREG0 0x38
#define MRF_BBREG1 0x39
#define MRF_BBREG2 0x3A
#define MRF_BBREG3 0x3B
#define MRF_BBREG4 0x3C
//#define MRF_Reserved 0x3D
#define MRF_BBREG6 0x3E
#define MRF_CCAEDTH 0x3F

#define MRF_RFCON0 0x200
#define MRF_RFCON1 0x201
#define MRF_RFCON2 0x202
#define MRF_RFCON3 0x203
#define MRF_RFCON5 0x205
#define MRF_RFCON6 0x206
#define MRF_RFCON7 0x207
#define MRF_RFCON8 0x208
#define MRF_SLPCAL0 0x209
#define MRF_SLPCAL1 0x20A
#define MRF_SLPCAL2 0x20B
#define MRF_RSSI 0x210
#define MRF_SLPCON0 0x211
#define MRF_SLPCON1 0x220
#define MRF_WAKETIMEL 0x222
#define MRF_WAKETIMEH 0x223
#define MRF_REMCNTL 0x224
#define MRF_REMCNTH 0x225
#define MRF_MAINCNT0 0x226
#define MRF_MAINCNT1 0x227
#define MRF_MAINCNT2 0x228
#define MRF_MAINCNT3 0x229
#define MRF_TESTMODE 0x22F
#define MRF_ASSOEADR1 0x231
#define MRF_ASSOEADR2 0x232
#define MRF_ASSOEADR3 0x233
#define MRF_ASSOEADR4 0x234
#define MRF_ASSOEADR5 0x235
#define MRF_ASSOEADR6 0x236
#define MRF_ASSOEADR7 0x237
#define MRF_ASSOSADR0 0x238
#define MRF_ASSOSADR1 0x239
#define MRF_UPNONCE0 0x240
#define MRF_UPNONCE1 0x241
#define MRF_UPNONCE2 0x242
#define MRF_UPNONCE3 0x243
#define MRF_UPNONCE4 0x244
#define MRF_UPNONCE5 0x245
#define MRF_UPNONCE6 0x246
#define MRF_UPNONCE7 0x247
#define MRF_UPNONCE8 0x248
#define MRF_UPNONCE9 0x249
#define MRF_UPNONCE10 0x24A
#define MRF_UPNONCE11 0x24B
#define MRF_UPNONCE12 0x24C

#define MRF_I_RXIF  0b00001000
#define MRF_I_TXNIF 0b00000001

extern void Delay(uint32_t nTime);
typedef struct {
    EZGPIO_Interface * cs;
    EZGPIO_Interface * reset;
    
} MRF24J40_t;

static MRF24J40_t mrf_driver;

static uint8_t read_short(uint8_t address);
static uint8_t read_long(uint16_t address);
static void write_short(uint8_t address, uint8_t data);
static void write_long(uint16_t address, uint8_t data);
static void set_channel(uint8_t channel);
static void rx_disable();

static void rx_enable();

// aMaxPHYPacketSize = 127, from the 802.15.4-2006 standard.
static uint8_t rx_buf[127];

// essential for obtaining the data frame only
// bytes_MHR = 2 Frame control + 1 sequence number + 2 panid + 2 shortAddr Destination + 2 shortAddr Source
#define BYTES_MHR  9
#define BYTES_FCS  2 // FCS length = 2
#define BYTES_NODATA (BYTES_MHR + BYTES_FCS); // no_data bytes in PHY payload,  header length + FCS

static int ignoreBytes = 0; // bytes to ignore, some modules behaviour.

static uint8_t bufPHY = 0; // flag to buffer all bytes in PHY Payload, or not

volatile uint8_t flag_got_rx;
volatile uint8_t flag_got_tx;

static rx_info_t rx_info;
static tx_info_t tx_info;

uint8_t len_of_str(char * str) {
    uint8_t len = 0;
    while(*str++) len+=1;
    return len;
}

static uint8_t read_short(uint8_t address) {
    uint8_t tx_d[] = {address<<1 & 0x7e, 0};
    uint8_t rx_d[2];
    EZGPIO_SetOutput(mrf_driver.cs, 0);
    spiReadWrite(SPI2, rx_d, tx_d, 2, SPI_SLOW);
    EZGPIO_SetOutput(mrf_driver.cs, 1);
    return rx_d[1]; //data only transmitted in 2nd half. section 2.14.1
}

static uint8_t read_long(uint16_t address) {
    uint16_t tx_dl = (((1 << 11) | (address << 1)) << 4);
    EZGPIO_SetOutput(mrf_driver.cs, 0);
    uint8_t rx;
    spiReadWrite16(SPI2, 0, &tx_dl, 1, SPI_SLOW);
    spiReadWrite(SPI2, &rx, 0, 1, SPI_SLOW);
    EZGPIO_SetOutput(mrf_driver.cs, 1);
    return rx;
}

static void write_short(uint8_t address, uint8_t data) {
    uint8_t tx_d[] = { ((address<<1 | 0x1) & 0x7f), data};
    EZGPIO_SetOutput(mrf_driver.cs, 0);
    spiReadWrite(SPI2, 0, tx_d, 2, SPI_SLOW);
    EZGPIO_SetOutput(mrf_driver.cs, 1);
}


static void write_long(uint16_t address, uint8_t data) {
    uint16_t tx_dl = (((1 << 11) | (address << 1) | 1) << 4);
    EZGPIO_SetOutput(mrf_driver.cs, 0);
    spiReadWrite16(SPI2, 0, &tx_dl, 1, SPI_SLOW);
    spiReadWrite(SPI2, 0, &data, 1, SPI_SLOW);
    EZGPIO_SetOutput(mrf_driver.cs, 1);
}

void mrf24j40_init(EZGPIO_Interface * cs, EZGPIO_Interface * reset) {
    mrf_driver.cs = cs;
    mrf_driver.reset = reset;
    
    spiInit(SPI2);
    EZGPIO_SetOutput(reset, 0);
    Delay(300);
    EZGPIO_SetOutput(reset, 1);
    
}

void mrf24j40_devinit() 
{
    
    write_short(MRF_PACON2, 0x98); // – Initialize FIFOEN = 1 and TXONTS = 0x6.
    write_short(MRF_TXSTBL, 0x95); // – Initialize RFSTBL = 0x9.

    
    write_long(MRF_RFCON0, 0x03); // – Initialize RFOPT = 0x03.
    write_long(MRF_RFCON1, 0x01); // – Initialize VCOOPT = 0x02.
    write_long(MRF_RFCON2, 0x80); // – Enable PLL (PLLEN = 1).
    write_long(MRF_RFCON6, 0x90); // – Initialize TXFIL = 1 and 20MRECVR = 1.
    write_long(MRF_RFCON7, 0x80); // – Initialize SLPCLKSEL = 0x2 (100 kHz Internal oscillator).
    write_long(MRF_RFCON8, 0x10); // – Initialize RFVCO = 1.
    write_long(MRF_SLPCON1, 0x21); // – Initialize CLKOUTEN = 1 and SLPCLKDIV = 0x01.

    //  Configuration for nonbeacon-enabled devices (see Section 3.8 “Beacon-Enabled and
    //  Nonbeacon-Enabled Networks”):
    write_short(MRF_BBREG2, 0x80); // Set CCA mode to ED
    write_short(MRF_CCAEDTH, 0x60); // – Set CCA ED threshold.
    write_short(MRF_BBREG6, 0x40); // – Set appended RSSI value to RXFIFO.
    set_channel(12);
    // max power is by default.. just leave it...
    // Set transmitter power - See “REGISTER 2-62: RF CONTROL 3 REGISTER (ADDRESS: 0x203)”.
    write_short(MRF_RFCTL, 0x04); //  – Reset RF state machine.
    write_short(MRF_RFCTL, 0x00); // part 2
    
    iprintf("Read shorts MRF_PACON2: 0x%x\r\n", read_short(MRF_PACON2));
    iprintf("Read shorts MRF_TXSTBL: 0x%x\r\n", read_short(MRF_TXSTBL));

}


uint8_t mrf24j40_getAckTMOut() {
    return read_short(MRF_ACKTMOUT);
}

static void set_channel(uint8_t channel) 
{
    write_long(MRF_RFCON0, (((channel - 11) << 4) | 0x03));
}

void mrf24j40_interrupt_handler(void) {
    uint8_t last_interrupt = read_short(MRF_INTSTAT);
    int i;
    if (last_interrupt & MRF_I_RXIF) {
        flag_got_rx++;
        // read out the packet data...
        //noInterrupts();
        rx_disable();
        // read start of rxfifo for, has 2 bytes more added by FCS. frame_length = m + n + 2
        uint8_t frame_length = read_long(0x300);

        // buffer all bytes in PHY Payload
        if(bufPHY){
            int rb_ptr = 0;
            for (i = 0; i < frame_length; i++) { // from 0x301 to (0x301 + frame_length -1)
                rx_buf[rb_ptr++] = read_long(0x301 + i);
            }
        }

        // buffer data bytes
        int rd_ptr = 0;
        // from (0x301 + bytes_MHR) to (0x301 + frame_length - bytes_nodata - 1)
        for (i = 0; i < rx_datalength(); i++) {
            rx_info.rx_data[rd_ptr++] = read_long(0x301 + BYTES_MHR + i);
        }

        rx_info.frame_length = frame_length;
        // same as datasheet 0x301 + (m + n + 2) <-- frame_length
        rx_info.lqi = read_long(0x301 + frame_length);
        // same as datasheet 0x301 + (m + n + 3) <-- frame_length + 1
        rx_info.rssi = read_long(0x301 + frame_length + 1);

        rx_enable();
        //interrupts();
    }
    if (last_interrupt & MRF_I_TXNIF) {
        flag_got_tx++;
        uint8_t tmp = read_short(MRF_TXSTAT);
        // 1 means it failed, we want 1 to mean it worked.
        tx_info.tx_ok = !(tmp & ~(1 << TXNSTAT));
        tx_info.retries = tmp >> 6;
        tx_info.channel_busy = (tmp & (1 << CCAFAIL));
    }
}


/**
 * Call this function periodically, it will invoke your nominated handlers
 */
mrf_evt mrf24j40_check_flags(void (*rx_handler)(void), void (*tx_handler)(void)){
    // TODO - we could check whether the flags are > 1 here, indicating data was lost?
    mrf_evt evt = mrf_noevent;
    if (flag_got_rx) {
        flag_got_rx = 0;
        evt |= mrf_rxevent;
    }
    if (flag_got_tx) {
        flag_got_tx = 0;
        evt |= mrf_txevent;
    }
    return evt;
}

/**
 * Set RX mode to promiscuous, or normal
 */
void set_promiscuous(uint8_t enabled) {
    if (enabled) {
        write_short(MRF_RXMCR, 0x01);
    } else {
        write_short(MRF_RXMCR, 0x00);
    }
}

rx_info_t * get_rxinfo(void) {
    return &rx_info;
}

tx_info_t * get_txinfo(void) {
    return &tx_info;
}

uint8_t * get_rxbuf(void) {
    return rx_buf;
}

int rx_datalength(void) {
    return rx_info.frame_length - BYTES_NODATA;
}

void set_ignoreBytes(int ib) {
    // some modules behaviour
    ignoreBytes = ib;
}

/**
 * Set bufPHY flag to buffer all bytes in PHY Payload, or not
 */
void set_bufferPHY(uint8_t bp) {
    bufPHY = bp;
}

uint8_t get_bufferPHY(void) {
    return bufPHY;
}

/**
 * Set PA/LNA external control
 */
void set_palna(uint8_t enabled) {
    if (enabled) {
        write_long(MRF_TESTMODE, 0x07); // Enable PA/LNA on MRF24J40MB module.
    }else{
        write_long(MRF_TESTMODE, 0x00); // Disable PA/LNA on MRF24J40MB module.
    }
}

static void rx_flush(void) {
    write_short(MRF_RXFLUSH, 0x01);
}

static void rx_disable() {
    write_short(MRF_BBREG1, 0x04);  // RXDECINV - disable receiver
}

static void rx_enable() {
    write_short(MRF_BBREG1, 0x00);  // RXDECINV - enable receiver
}


void mrf24j40_setpan(uint16_t pan_id) {
    write_short(MRF_PANIDH, pan_id >> 8);
    write_short(MRF_PANIDL, pan_id & 0xff);
}

uint16_t mrf24j40_getpan() {
    uint16_t panh = read_short(MRF_PANIDH);
    return panh << 8 | read_short(MRF_PANIDL);
}

uint16_t mrf24j40_getShortAddr() {
    uint16_t a16h = read_short(MRF_SADRH);
    return a16h << 8 | read_short(MRF_SADRL);
}

void mrf24j40_setShortAddr(uint16_t address16) 
{
    write_short(MRF_SADRH, address16 >> 8);
    write_short(MRF_SADRL, address16 & 0xff);
}

void mrf24j40_send16(uint16_t dest16, char* data) 
{
    uint8_t len = len_of_str(data); // get the length of the char* array
    int i = 0, q = 0;
    write_long(i++, BYTES_MHR); // header length
    // +ignoreBytes is because some module seems to ignore 2 bytes after the header?!.
    // default: ignoreBytes = 0;
    write_long(i++, BYTES_MHR+ignoreBytes+len);

    // 0 | pan compression | ack | no security | no data pending | data frame[3 bits]
    write_long(i++, 0b01100001); // first byte of Frame Control
    // 16 bit source, 802.15.4 (2003), 16 bit dest,
    write_long(i++, 0b10001000); // second byte of frame control
    write_long(i++, 1);  // sequence number 1

    uint16_t panid = mrf24j40_getpan();

    write_long(i++, panid & 0xff);  // dest panid
    write_long(i++, panid >> 8);
    write_long(i++, dest16 & 0xff);  // dest16 low
    write_long(i++, dest16 >> 8); // dest16 high

    uint16_t src16 = mrf24j40_getShortAddr();
    write_long(i++, src16 & 0xff); // src16 low
    write_long(i++, src16 >> 8); // src16 high

    // All testing seems to indicate that the next two bytes are ignored.
    //2 bytes on FCS appended by TXMAC
    i+=ignoreBytes;
    for (q = 0; q < len; q++) {
        write_long(i++, data[q]);
    }
    // ack on, and go!
    write_short(MRF_TXNCON, (1<<MRF_TXNACKREQ | 1<<MRF_TXNTRIG));
}
