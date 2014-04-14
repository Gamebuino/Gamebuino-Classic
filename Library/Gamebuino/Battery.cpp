/* 
 * File:   Battery.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 10:13 PM
 */

#include "Battery.h"

void Battery::begin() {
#if (ENABLE_BATTERY > 0)
    voltage = BAT_LVL_HIGH;
    level = NUM_LVL;
    nextUpdate = 1;
	show = false;
    thresolds[0] = BAT_LVL_LOW;
    thresolds[1] = BAT_LVL_MED;
    thresolds[2] = BAT_LVL_HIGH;
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
                /*if (level == 0) {//battery critic, power down
                    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
                    sleep_enable();
                    sleep_mode();
                }*/
                return;
            }
        }
    }
    nextUpdate++;
#endif
}

