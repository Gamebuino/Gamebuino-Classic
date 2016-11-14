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