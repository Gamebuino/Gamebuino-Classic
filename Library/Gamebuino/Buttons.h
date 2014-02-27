/* 
 * File:   Buttons.h
 * Author: Rodot
 *
 * Created on October 1, 2013, 8:53 PM
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
private:
    uint8_t pins[NUM_BTN];
    uint8_t states[NUM_BTN];

};

#endif	/* BUTTONS_H */

