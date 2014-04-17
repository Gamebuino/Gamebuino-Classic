/* 
 * File:   Buttons.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 8:53 PM
 */

#include "Buttons.h"

void Buttons::begin() {
    pins[BTN_LEFT] = BTN_LEFT_PIN;
    pins[BTN_UP] = BTN_UP_PIN;
    pins[BTN_RIGHT] = BTN_RIGHT_PIN;
    pins[BTN_DOWN] = BTN_DOWN_PIN;
    pins[BTN_A] = BTN_A_PIN;
    pins[BTN_B] = BTN_B_PIN;
    pins[BTN_C] = BTN_C_PIN;

    states[BTN_LEFT] = 0;
    states[BTN_UP] = 0;
    states[BTN_RIGHT] = 0;
    states[BTN_DOWN] = 0;
    states[BTN_A] = 0;
    states[BTN_B] = 0;
    states[BTN_C] = 0;
}

/*
 * reads each button states and store it
 */
void Buttons::update() {
    for (uint8_t thisButton = 0; thisButton < NUM_BTN; thisButton++) {
        pinMode(pins[thisButton], INPUT_PULLUP); //enable internal pull up resistors
        if (digitalRead(pins[thisButton]) == LOW) { //if button pressed
            states[thisButton]++; //increase button hold time
        } else {
            if (states[thisButton] == 0)//button idle
                continue;
            if (states[thisButton] == 0xFF)//if previously released
                states[thisButton] = 0; //set to idle
            else
                states[thisButton] = 0xFF; //button just released
        }
        pinMode(pins[thisButton], INPUT); //disable internal pull up resistors to save power
    }
	

}

/*
 * Returns true when 'button' is pressed.
 * The button has to be released for it to be triggered again.
 */
boolean Buttons::pressed(uint8_t button) {
    if (states[button] == 1)
        return true;
    else
        return false;
}

/*
 * return true if 'button' is released
 */
boolean Buttons::released(uint8_t button) {
    if (states[button] == 0xFF)
        return true;
    else
        return false;
}

/**
 * returns true ONCE when 'button' is held for 'time' frames
 * @param button The button's ID
 * @param time How much frames button must be held, between 1 and 254.
 * @return true when 'button' is held for 'time' frames
 */
boolean Buttons::held(uint8_t button, uint8_t time){
    if(states[button] == time)
        return true;
    else
        return false;
}

/**
 * returns true every 'period' frames when 'button' is held
 * @param button The button's ID
 * @param period How much frames button must be held, between 1 and 254.
 * @return true if the button is held for the given time
 */
boolean Buttons::repeat(uint8_t button, uint8_t period) {
    if (period <= 1) {
        if ((states[button] != 0xFF) && (states[button]))
            return true;
    } else {
        if ((states[button] != 0xFF) && ((states[button] % period) == 1))
            return true;
    }
    return false;
}

/**
 * 
 * @param button The button's ID
 * @return The number of frames during which the button has been held.
 */
uint8_t Buttons::timeHeld(uint8_t button){
    if(states[button] != 0xFF)
        return states[button];
    else
        return 0;
    
}
