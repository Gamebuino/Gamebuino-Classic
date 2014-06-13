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

class Sound {
public:
	void begin();

	void playTrack(const uint16_t* track, const uint16_t* const* instruments, uint8_t channel);
	void playTrack(const uint16_t* track, uint8_t channel);
	void updateTrack();
	void setTrackLooping(uint8_t channel, boolean loop);
	void stopTrack(uint8_t channel);
	void stopTrack();
	
	void playNote(uint8_t pitch, uint8_t duration, uint8_t channel);
	void updateNote();
	
	boolean isPlaying(uint8_t channel);
	void stopNote(uint8_t channel);
	void stopNote();

	void setVolume(int8_t volume);
	uint8_t getVolume();
	void setVolume(int8_t volume, uint8_t channel);
	uint8_t getVolume(uint8_t channel);
	
	void playOK();
	void playCancel();
	void playTick();
	
	uint8_t prescaler;

	void setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod);

	static void generateOutput(); //!\\ DO NOT USE
	uint8_t globalVolume;
	uint8_t volumeMax;
private:
#if (NUM_CHANNELS > 0)
	// track data
	uint16_t *trackData[NUM_CHANNELS];
	uint16_t **trackInstruments[NUM_CHANNELS];
	boolean trackLooping[NUM_CHANNELS];
	uint16_t trackCursor[NUM_CHANNELS];
	boolean trackPlaying[NUM_CHANNELS];

	// note data
	uint8_t notePitch[NUM_CHANNELS];
	uint8_t noteDuration[NUM_CHANNELS];
	int8_t noteVolume[NUM_CHANNELS];
	boolean notePlaying[NUM_CHANNELS];
	
	// commands data
	int8_t commandsCounter[NUM_CHANNELS];
	int8_t volumeSlideStepDuration[NUM_CHANNELS];
	int8_t volumeSlideStepSize[NUM_CHANNELS];
	uint8_t arpeggioStepDuration[NUM_CHANNELS];
	int8_t arpeggioStepSize[NUM_CHANNELS];
	uint8_t tremoloStepDuration[NUM_CHANNELS];
	int8_t tremoloStepSize[NUM_CHANNELS];
	
	
	// instrument data
	uint16_t *instrumentData[NUM_CHANNELS];
	uint8_t instrumentLength[NUM_CHANNELS]; //number of steps in the instrument
	uint8_t instrumentLooping[NUM_CHANNELS]; //how many steps to loop on when the last step of the instrument is reached
	uint16_t instrumentCursor[NUM_CHANNELS]; //which step is being played
	uint8_t instrumentNextChange[NUM_CHANNELS]; //how many frames before the next step
	
	//current step data
	int8_t stepVolume[NUM_CHANNELS];
	uint8_t stepPitch[NUM_CHANNELS];
	
	uint8_t chanVolumes[NUM_CHANNELS];
#endif
	static void updateOutput();
};

#endif	/* SOUND_H */

