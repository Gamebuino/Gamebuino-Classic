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

#define BUZZER_PIN 3 //not used, direct access to the register for perf


class Sound {
public:
    void begin();

    void play(uint16_t* soundData, uint8_t channel);
    boolean isPlaying(uint8_t channel);
    void stop(uint8_t channel);
    void stop();
    void setLooping(uint8_t channel, boolean loop);

    void setVolume(int8_t volume);
    uint8_t getVolume();
    void setVolume(int8_t volume, uint8_t channel);
    uint8_t getVolume(uint8_t channel);
	
	uint8_t prescaler;

    void playOK();
    void playCancel();
    void playTick();

    void update();
    void setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod);

    static void generateOutput(); //!\\ DO NOT USE
    uint8_t globalVolume;
	uint8_t volumeMax;
private:
#if (NUM_CHANNELS > 0)
    uint16_t* data[NUM_CHANNELS];
    uint8_t chanVolumes[NUM_CHANNELS];
    uint16_t dataLength[NUM_CHANNELS];
    uint16_t cursorPosition[NUM_CHANNELS];
    uint8_t nextChange[NUM_CHANNELS];
    boolean playing[NUM_CHANNELS];
    boolean looping[NUM_CHANNELS];
#endif
    static void updateOutput();
};

#endif	/* SOUND_H */

