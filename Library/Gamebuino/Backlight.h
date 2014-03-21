/* 
 * File:   Backlight.h
 * Author: Rodot
 *
 * Created on October 1, 2013, 5:40 PM
 */

#ifndef BACKLIGHT_H
#define	BACKLIGHT_H

#include <Arduino.h>
#include "settings.c"

class Backlight {
public:
    void begin(int8_t blacklight_pin);
    void set(uint8_t value);
    void setAuto(boolean val);
    void update();
    uint16_t ambientLight;
    uint8_t backlightValue;
private:
    int8_t blacklight_pin;
    boolean automatic;
    uint8_t smoothing;
};

#endif	/* BACKLIGHT_H */