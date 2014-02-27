/* 
 * File:   Backlight.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 5:40 PM
 */

#include "Backlight.h"

void Backlight::begin() {
    ambientLight = 0;
    automatic = true;
}

void Backlight::set(uint8_t value) {
    //analogWrite(BACKLIGHT_PIN, value);
}

void Backlight::setAuto(boolean val) {
    automatic = val;
}

void Backlight::update() {
    ambientLight = (ambientLight * (AMBIENTLIGHT_SMOOTHING - 1) + analogRead(AMBIENTLIGHT_PIN)) / AMBIENTLIGHT_SMOOTHING; //read value and smooth it
    if (automatic) {
        backlightValue = map(constrain(ambientLight, AMBIENTLIGHT_MIN, AMBIENTLIGHT_MAX), AMBIENTLIGHT_MIN, AMBIENTLIGHT_MAX, BACKLIGHT_MAX, BACKLIGHT_MIN);
        set(backlightValue);
    }
}