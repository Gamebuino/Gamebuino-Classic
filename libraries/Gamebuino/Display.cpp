/* 
 * File:   Display.cpp
 * Author: Rodot
 * 
 * Created on October 7, 2013, 12:05 PM
 */

#include "Display.h"

// a 5x7 font table
extern uint8_t PROGMEM font[];

// the memory buffer for the LCD
uint8_t _displayBuffer[LCDWIDTH * LCDHEIGHT / 8];

void Display::begin(int8_t SCLK, int8_t DIN, int8_t DC, int8_t CS, int8_t RST) {
    din = DIN;
    sclk = SCLK;
    dc = DC;
    rst = RST;
    cs = CS;

    cursor_y = cursor_x = 0;
    textsize = 1;
    color = BLACK;
	bgcolor = WHITE;
    wrap = true;
	persistence = false;

    SPI.begin();
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV4);
    SPI.setDataMode(SPI_MODE3);
    // set pin directions
    pinMode(din, OUTPUT);
    pinMode(sclk, OUTPUT);
    pinMode(dc, OUTPUT);
    if (rst > 0)
        pinMode(rst, OUTPUT);
    if (cs > 0)
        pinMode(cs, OUTPUT);

    // toggle RST low to reset
    if (rst > 0) {
        digitalWrite(rst, LOW);
        delay(5);
        digitalWrite(rst, HIGH);
    }

    clkport = portOutputRegister(digitalPinToPort(sclk));
    clkpinmask = digitalPinToBitMask(sclk);
    mosiport = portOutputRegister(digitalPinToPort(din));
    mosipinmask = digitalPinToBitMask(din);
    csport = portOutputRegister(digitalPinToPort(cs));
    cspinmask = digitalPinToBitMask(cs);
    dcport = portOutputRegister(digitalPinToPort(dc));
    dcpinmask = digitalPinToBitMask(dc);

    // get into the EXTENDED mode!
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);

    // LCD bias select (4 is optimal?)
    command(PCD8544_SETBIAS | 0x4);

    // set VOP
    if (contrast > 0x7f)
        contrast = 0x7f;

    command(PCD8544_SETVOP | contrast); // Experimentally determined


    // normal mode
    command(PCD8544_FUNCTIONSET);

    // Set display to Normal
    command(PCD8544_DISPLAYCONTROL | PCD8544_DISPLAYNORMAL);

    // initial display line
    // set page address
    // set column address
    // write display data

    update();
}

void Display::command(uint8_t c) {
    noInterrupts();
    digitalWrite(dc, LOW);
    if (cs > 0)
        digitalWrite(cs, LOW);
    SPI.transfer((char) c);
    if (cs > 0)
        digitalWrite(cs, HIGH);
    interrupts();
}

void Display::data(uint8_t c) {
    noInterrupts();
    digitalWrite(dc, HIGH);
    if (cs > 0)
        digitalWrite(cs, LOW);
    SPI.transfer((char) c);
    if (cs > 0)
        digitalWrite(cs, HIGH);
    interrupts();
}

void Display::setContrast(uint8_t val) {
	contrast = constrain(val, 50, 70);
    /*if (contrast > 0x7f) {
        contrast = 0x7f;
    }*/
    command(PCD8544_FUNCTIONSET | PCD8544_EXTENDEDINSTRUCTION);
    command(PCD8544_SETVOP | contrast);
    command(PCD8544_FUNCTIONSET);

}

void Display::clear(void) {
    memset(_displayBuffer, 0, LCDWIDTH * LCDHEIGHT / 8);
    cursor_y = cursor_x = 0;
}

void Display::fillScreen(uint8_t color) {
    memset(_displayBuffer, 0xFFFF, LCDWIDTH * LCDHEIGHT / 8);
}

void Display::update(void) {
    uint8_t col, maxcol, p;

    for (p = 0; p < 6; p++) {

        command(PCD8544_SETYADDR | p);

        // start at the beginning of the row
        col = 0;
        maxcol = LCDWIDTH_NOROT - 1;

        command(PCD8544_SETXADDR | col);

        digitalWrite(dc, HIGH);
        if (cs > 0)
            digitalWrite(cs, LOW);
        for (; col <= maxcol; col++) {
            SPI.transfer(_displayBuffer[(LCDWIDTH_NOROT * p) + col]);
        }

        if (cs > 0)
            digitalWrite(cs, HIGH);

    }

    command(PCD8544_SETYADDR); // no idea why this is necessary but it is to finish the last byte?
}

void Display::setColor(int8_t c){
	color = c;
	bgcolor = c;
}

void Display::setColor(int8_t c, int8_t bg){
	color = c;
	bgcolor = bg;
}

void Display::drawPixel(int8_t x, int8_t y) {
    if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
        return;

#if DISPLAY_ROT == NOROT
    if (color)
        _displayBuffer[x + (y / 8) * LCDWIDTH_NOROT] |= _BV(y % 8);
    else
        _displayBuffer[x + (y / 8) * LCDWIDTH_NOROT] &= ~_BV(y % 8);
#elif DISPLAY_ROT == ROTCCW
	if (color)
        _displayBuffer[LCDHEIGHT - y - 1 + (x / 8) * LCDWIDTH_NOROT] |= _BV(x % 8);
    else
        _displayBuffer[LCDHEIGHT - y - 1 + (x / 8) * LCDWIDTH_NOROT] &= ~_BV(x % 8);
#elif DISPLAY_ROT == ROT180
	if (color)
        _displayBuffer[LCDWIDTH - x - 1 + ((LCDHEIGHT - y - 1) / 8) * LCDWIDTH_NOROT] |= _BV((LCDHEIGHT - y - 1) % 8);
    else
        _displayBuffer[LCDWIDTH - x - 1 + ((LCDHEIGHT - y - 1) / 8) * LCDWIDTH_NOROT] &= ~_BV((LCDHEIGHT - y - 1) % 8);
#elif DISPLAY_ROT == ROTCW
	if (color)
        _displayBuffer[y + ((LCDWIDTH - x - 1) / 8) * LCDWIDTH_NOROT] |= _BV((LCDWIDTH - x -1) % 8);
    else
        _displayBuffer[y + ((LCDWIDTH - x - 1) / 8) * LCDWIDTH_NOROT] &= ~_BV((LCDWIDTH - x - 1) % 8);
#endif
}

uint8_t Display::getPixel(int8_t x, int8_t y) {
    if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
        return 0;

    return (_displayBuffer[x + (y / 8) * LCDWIDTH] >> (y % 8)) & 0x1;
}

void Display::drawFastVLine(int8_t x, int8_t y, int8_t h) {
    // stupidest/slowest version :
    drawLine(x, y, x, y + h - 1);
    /*if ((x < 0) || (x >= LCDWIDTH) || ((y + h) < 0) || (y >= LCDHEIGHT))
        return;
    if(y < 0){
        h += y;
        y = 0;
    }
    h = (y + h > LCDHEIGHT) ? LCDHEIGHT - y : h;
    
    for (uint8_t y2 = y; y2 < y + h; y2++) {
        if (color)
            _displayBuffer[x + (y2 / 8) * LCDWIDTH] |= _BV(y2 % 8);
        else
            _displayBuffer[x + (y2 / 8) * LCDWIDTH] &= ~_BV(y2 % 8);
    }*/
}

void Display::drawFastHLine(int8_t x, int8_t y, int8_t w) {
    // stupidest/slowest version :
    drawLine(x, y, x + w - 1, y);
    /*if (((x+w) < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
        return;
    if(x < 0){
        w += x;
        x = 0;
    }
    w = (x + w > LCDWIDTH) ? LCDWIDTH - x : w;

    for (uint8_t x2 = x; x2 < x + w; x2++) {
        if (color)
            _displayBuffer[x2 + (y / 8) * LCDWIDTH] |= _BV(y % 8);
        else
            _displayBuffer[x2 + (y / 8) * LCDWIDTH] &= ~_BV(y % 8);
    }*/
    //should be faster but behaves weirdly
    //memset(&_displayBuffer[x + (y / 8) * LCDWIDTH], color, w);
}

void Display::drawRect(int8_t x, int8_t y, int8_t w, int8_t h) {

    drawFastHLine(x, y, w);
    drawFastHLine(x, y + h - 1, w);
    drawFastVLine(x, y, h);
    drawFastVLine(x + w - 1, y, h);
}

void Display::fillRect(int8_t x, int8_t y, int8_t w, int8_t h) {
    // stupidest version - update in subclasses if desired!
    for (int8_t i = x; i < x + w; i++) {
        drawFastVLine(i, y, h);
    }
}

void Display::drawLine(int8_t x0, int8_t y0, int8_t x1, int8_t y1) {
    int8_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        swap(x0, y0);
        swap(x1, y1);
    }

    if (x0 > x1) {
        swap(x0, x1);
        swap(y0, y1);
    }

    int8_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int8_t err = dx / 2;
    int8_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0);
        } else {
            drawPixel(x0, y0);
        }
        err -= dy;
        if (err < 0) {

            y0 += ystep;
            err += dx;
        }
    }
}

void Display::drawCircle(int8_t x0, int8_t y0, int8_t r) {
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    drawPixel(x0, y0 + r);
    drawPixel(x0, y0 - r);
    drawPixel(x0 + r, y0);
    drawPixel(x0 - r, y0);

    while (x < y) {
        if (f >= 0) {

            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y);
        drawPixel(x0 - x, y0 + y);
        drawPixel(x0 + x, y0 - y);
        drawPixel(x0 - x, y0 - y);
        drawPixel(x0 + y, y0 + x);
        drawPixel(x0 - y, y0 + x);
        drawPixel(x0 + y, y0 - x);
        drawPixel(x0 - y, y0 - x);

    }
}

void Display::drawCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername) {
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        if (cornername & 0x4) {
            drawPixel(x0 + x, y0 + y);
            drawPixel(x0 + y, y0 + x);
        }
        if (cornername & 0x2) {
            drawPixel(x0 + x, y0 - y);
            drawPixel(x0 + y, y0 - x);
        }
        if (cornername & 0x8) {
            drawPixel(x0 - y, y0 + x);
            drawPixel(x0 - x, y0 + y);
        }
        if (cornername & 0x1) {

            drawPixel(x0 - y, y0 - x);
            drawPixel(x0 - x, y0 - y);
        }
    }
}

void Display::fillCircle(int8_t x0, int8_t y0, int8_t r) {
    drawFastVLine(x0, y0 - r, 2 * r + 1);
    fillCircleHelper(x0, y0, r, 3, 0);
}

// used to do circles and roundrects!
void Display::fillCircleHelper(int8_t x0, int8_t y0, int8_t r, uint8_t cornername, int8_t delta) {
    int8_t f = 1 - r;
    int8_t ddF_x = 1;
    int8_t ddF_y = -2 * r;
    int8_t x = 0;
    int8_t y = r;

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        if (cornername & 0x1) {
            drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta);
            drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta);
        }
        if (cornername & 0x2) {

            drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta);
            drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta);
        }
    }
}

void Display::drawRoundRect(int8_t x, int8_t y, int8_t w,int8_t h, int8_t r) {
    // smarter version
    drawFastHLine(x + r, y, w - 2 * r); // Top
    drawFastHLine(x + r, y + h - 1, w - 2 * r); // Bottom
    drawFastVLine(x, y + r, h - 2 * r); // Left
    drawFastVLine(x + w - 1, y + r, h - 2 * r); // Right
    // draw four corners
    drawCircleHelper(x + r, y + r, r, 1);
    drawCircleHelper(x + w - r - 1, y + r, r, 2);
    drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4);
    drawCircleHelper(x + r, y + h - r - 1, r, 8);
}

void Display::fillRoundRect(int8_t x, int8_t y, int8_t w,int8_t h, int8_t r) {
    fillRect(x + r, y, w - 2 * r, h);
    // draw four corners
    fillCircleHelper(x + w - r - 1, y + r, r, 1, h - 2 * r - 1);
    fillCircleHelper(x + r, y + r, r, 2, h - 2 * r - 1);
}

void Display::drawTriangle(int8_t x0, int8_t y0,
        int8_t x1, int8_t y1,
        int8_t x2, int8_t y2) {
    drawLine(x0, y0, x1, y1);
    drawLine(x1, y1, x2, y2);
    drawLine(x2, y2, x0, y0);
}

void Display::fillTriangle(int8_t x0, int8_t y0,
        int8_t x1, int8_t y1,
        int8_t x2, int8_t y2) {
    int8_t a, b, y, last;

    // Sort coordinates by Y order (y2 >= y1 >= y0)
    if (y0 > y1) {
        swap(y0, y1);
        swap(x0, x1);
    }
    if (y1 > y2) {
        swap(y2, y1);
        swap(x2, x1);
    }
    if (y0 > y1) {
        swap(y0, y1);
        swap(x0, x1);
    }

    if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
        a = b = x0;
        if (x1 < a) a = x1;
        else if (x1 > b) b = x1;
        if (x2 < a) a = x2;
        else if (x2 > b) b = x2;
        drawFastHLine(a, y0, b - a + 1);
        return;
    }

    int16_t
    dx01 = x1 - x0,
            dy01 = y1 - y0,
            dx02 = x2 - x0,
            dy02 = y2 - y0,
            dx12 = x2 - x1,
            dy12 = y2 - y1,
            sa = 0,
            sb = 0;

    // For upper part of triangle, find scanline crossings for segments
    // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
    // is included here (and second loop will be skipped, avoiding a /0
    // error there), otherwise scanline y1 is skipped here and handled
    // in the second loop...which also avoids a /0 error here if y0=y1
    // (flat-topped triangle).
    if (y1 == y2) last = y1; // Include y1 scanline
    else last = y1 - 1; // Skip it

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        /* longhand:
        a = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b) swap(a, b);
        drawFastHLine(a, y, b - a + 1);
    }

    // For lower part of triangle, find scanline crossings for segments
    // 0-2 and 1-2.  This loop is skipped if y1=y2.
    sa = dx12 * (y - y1);
    sb = dx02 * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;

        /* longhand:
                a = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                b = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
         */
        if (a > b) swap(a, b);
        drawFastHLine(a, y, b - a + 1);
    }
}

void Display::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap) {
	int8_t w = pgm_read_byte(bitmap);
	int8_t h = pgm_read_byte(bitmap + 1);
#if (ENABLE_BITMAPS > 0)
    int8_t i, j, byteWidth = (w + 7) / 8;
    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            if (pgm_read_byte(2 + bitmap + j * byteWidth + i / 8) & (B10000000 >> (i % 8))) {
                drawPixel(x + i, y + j);
            }
        }
    }
#else
	drawRect(x, y, w, h);
#endif
}

void Display::drawBitmap(int8_t x, int8_t y, const uint8_t *bitmap,
        uint8_t rotation, uint8_t flip) {
	int8_t w = pgm_read_byte(bitmap);
	int8_t h = pgm_read_byte(bitmap + 1);
#if (ENABLE_BITMAPS > 0)
    int8_t i, j, //coordinates in the raw bitmap
            k, l, //coordinates in the rotated/flipped bitmap
            byteWidth = (w + 7) / 8;

    rotation %= 4;

    for (j = 0; j < h; j++) {
        for (i = 0; i < w; i++) {
            if (pgm_read_byte(2 + bitmap + j * byteWidth + i / 8) & (B10000000 >> (i % 8))) {
                switch (rotation) {
                    case NOROT: //no rotation
                        k = i;
                        l = j;
                        break;
                    case ROTCCW: //90° counter-clockwise
                        k = j;
                        l = w - i - 1;
                        break;
                    case ROT180: //180°
                        k = w - i - 1;
                        l = h - j - 1;
                        break;
                    case ROTCW: //90° clockwise
                        k = h - j - 1;
                        l = i;
                        break;
                }
                if (flip) {
                    flip %= 4;
                    if (flip & B00000001) { //horizontal flip
                        k = w - k;
                    }
                    if (flip & B00000010) { //vertical flip
                        l = h - l;
                    }
                }
                k += x; //place the bitmap on the screen
                l += y;
                drawPixel(k, l);
            }
        }
    }
#else
	drawRect(x, y, w, h);
#endif
}

#if ARDUINO >= 100

size_t Display::write(uint8_t c) {
#else

void Display::write(uint8_t c) {
#endif
    if (c == '\n') {
        cursor_y += textsize * FONTHEIGHT;
        cursor_x = 0;
    } else if (c == '\r') {
        // skip em
    } else {
        drawChar(cursor_x, cursor_y, c, textsize);
        cursor_x += textsize * FONTWIDTH;
        if (wrap && (cursor_x > (LCDWIDTH - textsize * FONTWIDTH))) {
            cursor_y += textsize * FONTHEIGHT;
            cursor_x = 0;
        }
    }
#if ARDUINO >= 100
    return 1;
#endif
}

void Display::drawChar(int8_t x, int8_t y, unsigned char c, uint8_t size) {
	int8_t tempcolor = color;

    if ((x >= LCDWIDTH) || // Clip right
            (y >= LCDHEIGHT) || // Clip bottom
            ((x + (FONTWIDTH-1) * size - 1) < 0) || // Clip left
            ((y + FONTHEIGHT * size - 1) < 0)) // Clip top
        return;

    for (int8_t i = 0; i < FONTWIDTH; i++) {
        uint8_t line;
        if (i == (FONTWIDTH-1))
            line = 0x0;
        else
            line = pgm_read_byte(font + (c * (FONTWIDTH-1)) + i);
        for (int8_t j = 0; j < FONTHEIGHT; j++) {
            if (line & 0x1) {
                if (size == 1) // default size
                    drawPixel(x + i, y + j);
                else { // big size
                    fillRect(x + (i * size), y + (j * size), size, size);
                }
            } else if (bgcolor != color) {
				color = bgcolor;
                if (size == 1) // default size
                    drawPixel(x + i, y + j);
                else { // big size
                    fillRect(x + i*size, y + j*size, size, size);
                }
			    color = tempcolor; //restore color to its initial value
            }
            line >>= 1;
        }
    }
	//restore color to what it was previously
	color = tempcolor;
}

void Display::setCursor(int8_t x, int8_t y) {
    cursor_x = x;
    cursor_y = y;
}

void Display::setTextSize(uint8_t s) {
    textsize = (s > 0) ? s : 1;
}

void Display::setTextWrap(boolean w) {
    wrap = w;
}