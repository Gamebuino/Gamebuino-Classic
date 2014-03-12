/* 
 * File:   Battery.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 10:13 PM
 */

#include "Battery.h"

void Battery::begin() {
    voltage = 0;
    level = 0;
    nextUpdate = 0;
	show = true;
    thresolds[0] = BAT_LVL_LOW;
    thresolds[1] = BAT_LVL_MED;
    thresolds[2] = BAT_LVL_HIGH;
}

void Battery::update() {
    if (!(nextUpdate % 20)) { //every 20 frames
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
}

uint16_t Battery::getVoltage(){
    return voltage;
}

uint8_t Battery::getLevel(){
    return level;
}

void Battery::display(boolean disp){
	show = disp;
}

