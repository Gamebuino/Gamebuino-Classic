/* 
 * File:   Gamebuino.h
 * Author: Rodot
 *
 * Created on October 1, 2013, 5:39 PM
 */

#ifndef GAMEBUINO_H
#define	GAMEBUINO_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include "settings.c"
#include "Backlight.h"
#include "Display.h"
#include "Buttons.h"
#include "Battery.h"
#include "Sound.h"

#define load_game (*((void(*)(const char* filename))(0x7ffc/2)))
#define write_flash_page (*((void(*)(prog_char * page, unsigned char * buffer))(0x7ffa/2)))

class Gamebuino {
public:
    Gamebuino();
    Backlight backlight;
    Buttons buttons;
    Battery battery;
    Sound sound;
    Display display;

	void begin(const __FlashStringHelper* name, const uint8_t *logo);
	void begin(const __FlashStringHelper* name);
    void begin();
    boolean update();
	boolean startMenuTimer;
    uint32_t frameCount;
    void setFrameRate(uint8_t fps);
	uint8_t getCpuLoad();
    
    uint16_t freeRam();
    uint16_t frameDurationMicros;
    uint32_t frameStartMicros, frameEndMicros;
    
    int8_t menu(char** items, uint8_t length);
    void keyboard(char* text, uint8_t length);
    void popup(const __FlashStringHelper* text, uint8_t duration);
	void adjustVolume();
	void changeGame();
	boolean settingsAvailable();
	void readSettings();
	void getDefaultName(char* string);
	
	boolean collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h);
	boolean collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2);
    
private:
    uint8_t timePerFrame;
    uint32_t nextFrameMillis;
    void updatePopup();
    const __FlashStringHelper* popupText;
    uint8_t popupTimeLeft;
    void displayBattery();
};

#endif	/* GAMEBUINO_H */

