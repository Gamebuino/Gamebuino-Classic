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

#ifndef BATTERY_H
#define	BATTERY_H

#include <Arduino.h>
#include "settings.c"

/*
//used to power down when battery is too low
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
 */


class Battery {
public:
    void begin();
    void update();
	boolean show;
    uint8_t  level;
    uint16_t voltage;
    uint16_t thresholds[NUM_LVL];
    uint8_t  nextUpdate;
};

#endif	/* BATTERY_H */

