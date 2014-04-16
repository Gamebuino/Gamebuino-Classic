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
    void begin();
    void set(uint8_t value);
    void update();
    boolean automatic;
    uint16_t ambientLight;
    uint8_t backlightValue;
	uint8_t backlightMin, backlightMax;
	uint16_t ambientLightMin, ambientLightMax;
};

#endif	/* BACKLIGHT_H */