/* 
 * File:   Battery.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 10:13 PM
 */

#include "Battery.h"

void Battery::begin() {
#if (ENABLE_BATTERY > 0)
    voltage = BAT_LVL_FULL;
    level = NUM_LVL;
    nextUpdate = 1;
	show = false;
#endif
}

void Battery::update() {
#if (ENABLE_BATTERY > 0)
    if (!(nextUpdate % 32)) { //every 32 frames
        voltage = analogRead(BAT_PIN)*6.4453; //3.3V * 2 *1000 / 1024
        //set the battery 'level' according to thresholds
        level = NUM_LVL;
        for (uint8_t i = 0; i < NUM_LVL; i++) {
            if (voltage < thresolds[i]) {
                level = i;
                return;
            }
        }
    }
    nextUpdate++;
#endif
}

