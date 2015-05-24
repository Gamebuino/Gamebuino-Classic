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

#ifndef BUTTONS_H
#define	BUTTONS_H

#include <Arduino.h>
#include "settings.c"

class Buttons {
public:
    void begin();
    void update();
    boolean pressed(uint8_t button);
    boolean released(uint8_t button);
    boolean held(uint8_t button, uint8_t time);
    boolean repeat(uint8_t button, uint8_t period);
    uint8_t timeHeld(uint8_t button);
    uint8_t pins[NUM_BTN];
    uint8_t states[NUM_BTN];

};

#endif	/* BUTTONS_H */

