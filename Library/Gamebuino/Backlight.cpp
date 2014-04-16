/* 
 * File:   Backlight.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 5:40 PM
 */

#include "Backlight.h"

void Backlight::begin() {
#if (ENABLE_BACKLIGHT > 0)
    ambientLight = 0;
    automatic = true;
#endif
}

void Backlight::set(uint8_t value) {
    analogWrite(BACKLIGHT_PIN, value);
}

void Backlight::update() {
#if (ENABLE_BACKLIGHT > 0)
    ambientLight = (ambientLight * (AMBIENTLIGHT_SMOOTHING - 1) + analogRead(AMBIENTLIGHT_PIN)) / AMBIENTLIGHT_SMOOTHING; //read value and smooth it
    if (automatic) {
        backlightValue = map(constrain(ambientLight, ambientLightMin, ambientLightMax), ambientLightMin, ambientLightMax, backlightMax, backlightMin);
        set(backlightValue);
    }
#endif
}