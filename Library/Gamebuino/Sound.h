/* 
 * File:   Sound.h
 * Author: Rodot
 *
 * Created on October 2, 2013, 5:11 PM
 */

#ifndef SOUND_H
#define	SOUND_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "settings.c"

#define BUZZER_PIN 3 //not used, direct acces to the register for perf
#define NUM_CHANNELS 1

class Sound {
public:
    void begin();

    void play(uint16_t* soundData, uint8_t channel);
    boolean isPlaying(uint8_t channel);
    void stop(uint8_t channel);
    void stop();
    void setLooping(uint8_t channel, boolean loop);

    void setGlobalVolume(int8_t volume);
    uint8_t getGlobalVolume();
    void setChannelVolume(int8_t volume, uint8_t channel);
    uint8_t getChannelVolume(uint8_t channel);

    void playOK();
    void playCancel();
    void playTick();

    void update();
    void setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod);

    static void generateOutput(); //!\\ DO NOT USE
private:
    uint16_t* data[NUM_CHANNELS];
    uint8_t globalVolume;
    uint8_t chanVolumes[NUM_CHANNELS];
    uint16_t dataLength[NUM_CHANNELS];
    uint16_t cursorPosition[NUM_CHANNELS];
    uint8_t nextChange[NUM_CHANNELS];
    boolean playing[NUM_CHANNELS];
    boolean looping[NUM_CHANNELS];
    static void updateOutput();

};

#endif	/* SOUND_H */

