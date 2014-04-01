/* 
 * File:   Display.h
 * Author: Rodot
 * Library adapted and heavily optimized from https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library
 * 
 * Created on October 7, 2013, 12:05 PM
 */

#ifndef DISPLAY_H
#define	DISPLAY_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <SPI.h>

#include "font5x7.c"
#include "settings.c"

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 84
#define LCDHEIGHT 48

//for extended bitmap function :
#define NOROT 0
#define ROTCCW 1
#define ROT180 2
#define ROTCW 3
#define NOFLIP 0
#define FLIPH 1
#define FLIPV 2
#define FLIPVH 3

#define swap(a, b) { int8_t t = a; a = b; b = t; }

#define PCD8544_POWERDOWN 0x04
#define PCD8544_ENTRYMODE 0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK 0x0
#define PCD8544_DISPLAYNORMAL 0x4
#define PCD8544_DISPLAYALLON 0x1
#define PCD8544_DISPLAYINVERTED 0x5

// H = 0
#define PCD8544_FUNCTIONSET 0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR 0x40
#define PCD8544_SETXADDR 0x80
#define PCD8544_SETXADDR 0x80

// H = 1
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP 0x80

class Display : public Print {
public:
    void begin(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST);

    void command(uint8_t c);
    void data(uint8_t c);

    void setContrast(uint8_t val);
    void clear(void);
    void update();

    void drawPixel(int8_t x, int8_t y, uint8_t color);
    uint8_t getPixel(int8_t x, int8_t y);

    void drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1, uint8_t color);
    void drawFastVLine(int8_t x, int8_t y, int8_t h, uint8_t color);
    void drawFastHLine(int8_t x, int8_t y, int8_t w, uint8_t color);
    void drawRect(int8_t x, int8_t y, int8_t w, int8_t h, uint8_t color);
    void fillRect(int8_t x, int8_t y, int8_t w, int8_t h, uint8_t color);
    void fillScreen(uint8_t color);

    void drawCircle(int8_t x0, int8_t y0, int8_t r, uint8_t color);
    void drawCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername, uint8_t color);
    void fillCircle(int8_t x0, int8_t y0, int8_t r, uint8_t color);
    void fillCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername, int8_t delta, uint8_t color);

    void drawTriangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int8_t x2, int8_t y2, uint8_t color);
    void fillTriangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int8_t x2, int8_t y2, uint8_t color);
    void drawRoundRect(int8_t x0, int8_t y0, int8_t w, int8_t h, int8_t radius, uint8_t color);
    void fillRoundRect(int8_t x0, int8_t y0, int8_t w, int8_t h, int8_t radius, uint8_t color);
    
    void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap);
    void drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap, uint8_t rotation, uint8_t flip);
	void setBitmapColor(uint8_t c);
    void drawChar(int8_t x, int8_t y, unsigned char c, uint8_t color, uint8_t bg, uint8_t size);

    virtual size_t write(uint8_t);
    void setCursor(int8_t x, int8_t y);
    void setTextColor(uint8_t c);
    void setTextColor(uint8_t c, uint8_t bg);
    void setTextSize(uint8_t s);
    void setTextWrap(boolean w);
	
	boolean persistence; //disable automatic display clean() at each frame if true

private:
    int8_t sclk, din, dc, cs, rst;
    volatile uint8_t *mosiport, *clkport, *csport, *dcport;
    uint8_t mosipinmask, clkpinmask, cspinmask, dcpinmask;
	

    int8_t cursor_x, cursor_y;
	uint8_t bitmapcolor;
    uint8_t textcolor, textbgcolor;
    uint8_t textsize;
    boolean wrap; // If set, 'wrap' text at right edge of display
};

#endif	/* DISPLAY_H */

