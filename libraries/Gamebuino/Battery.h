/* 
 * File:   Battery.h
 * Author: Rodot
 *
 * Created on October 1, 2013, 10:13 PM
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
    uint16_t thresolds[NUM_LVL];
    uint8_t  nextUpdate;
};

#endif	/* BATTERY_H */

