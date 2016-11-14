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

#ifndef SOUND_H
#define	SOUND_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "settings.c"

//commands
#define CMD_VOLUME 0
#define CMD_INSTRUMENT 1
#define CMD_SLIDE 2
#define CMD_ARPEGGIO 3
#define CMD_TREMOLO 4

class Sound {
public:
	void begin();
	
	void playTrack(const uint16_t* track, uint8_t channel);
	void updateTrack(uint8_t channel);
	void updateTrack();
	void stopTrack(uint8_t channel);
	void stopTrack();
	void changePatternSet(const uint16_t* const* patterns, uint8_t channel);
	boolean trackIsPlaying[NUM_CHANNELS];
	
	void playPattern(const uint16_t* pattern, uint8_t channel);
	void changeInstrumentSet(const uint16_t* const* instruments, uint8_t channel);
	void updatePattern(uint8_t i);
	void updatePattern();
	void setPatternLooping(boolean loop, uint8_t channel);
	void stopPattern(uint8_t channel);
	void stopPattern();
	boolean patternIsPlaying[NUM_CHANNELS];
	
	void command(uint8_t cmd, uint8_t X, int8_t Y, uint8_t i);
	void playNote(uint8_t pitch, uint8_t duration, uint8_t channel);
	void updateNote();
	void updateNote(uint8_t i);
	void stopNote(uint8_t channel);
	void stopNote();
	
	uint8_t outputPitch[NUM_CHANNELS];
	int8_t outputVolume[NUM_CHANNELS];

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
	
//the following is no longer private, but...
//with great powers comes great responsibility.
//it up to you to mess everything up and burn your speaker
//private:
#if (NUM_CHANNELS > 0)
	//tracks data
	uint16_t *trackData[NUM_CHANNELS];
	uint8_t trackCursor[NUM_CHANNELS];
	uint16_t **patternSet[NUM_CHANNELS];
	int8_t patternPitch[NUM_CHANNELS];

	// pattern data
	uint16_t *patternData[NUM_CHANNELS];
	uint16_t **instrumentSet[NUM_CHANNELS];
	boolean patternLooping[NUM_CHANNELS];
	uint16_t patternCursor[NUM_CHANNELS];

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

