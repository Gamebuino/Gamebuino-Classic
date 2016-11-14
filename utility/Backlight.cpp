/*
 * (C) Copyright 2014 Aurélien Rodot. All rights reserved.
 *
 * This file is part of the Gamebuino Library (http://gamebuino.com)
 *
 * The Gamebuino Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "Backlight.h"

void Backlight::begin() {
#if (ENABLE_BACKLIGHT > 0)
    //ambientLight = 0;
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