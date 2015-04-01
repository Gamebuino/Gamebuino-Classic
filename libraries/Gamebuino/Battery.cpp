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

#include "Battery.h"

void Battery::begin() {
#if (ENABLE_BATTERY > 0)
    voltage = BAT_LVL_FULL;
    level = NUM_LVL;
    nextUpdate = 1;
	//show = false;
#endif
}

void Battery::update() {
#if (ENABLE_BATTERY > 0)
    if (!(nextUpdate % 32)) { //every 32 frames
        voltage = analogRead(BAT_PIN)*6.4453; //3.3V * 2 *1000 / 1024
		if(voltage){
        //set the battery 'level' according to thresholds
        level = NUM_LVL;
            for (uint8_t i = 0; i < NUM_LVL; i++) {
                if (voltage < thresolds[i]) {
                    level = i;
                    return;
                }
            }
	 	}
		else{
			level = 255;
		}
    }
    nextUpdate++;
#endif
}

