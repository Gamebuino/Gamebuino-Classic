/* 
* File:   Sound.cpp
* Author: Rodot
* 
* Created on October 2, 2013, 5:11 PM
*/

#include "Sound.h"
#if(NUM_CHANNELS > 0)
//declare these variables globally for faster access
uint8_t _rand = 1;
uint8_t _chanCount[NUM_CHANNELS]; //counts until the next change of the waveform
boolean _chanState[NUM_CHANNELS]; //if the waveform is currently high or low
uint8_t _chanHalfPeriod[NUM_CHANNELS]; //duration of half the period of the waveform
uint8_t _chanOutputVolume[NUM_CHANNELS]; //amplitude of the outputted waveform
uint8_t _chanOutput[NUM_CHANNELS]; //current value of the outputted waveform
boolean _chanNoise[NUM_CHANNELS]; //if a random value should be added to the waveform to generate noise


const uint16_t squareWaveInstrument[] PROGMEM = {0x0101, 0x03F7};
const uint16_t noiseInstrument[] PROGMEM = {0x0101, 0x03FF};
const uint16_t *defaultInstruments[] PROGMEM = {squareWaveInstrument,noiseInstrument};

const uint16_t playOKTrack[] PROGMEM = {0x0005,0x138,0x168,0x0000};
const uint16_t playCancelTrack[] PROGMEM = {0x0005,0x168,0x138,0x0000};
const uint16_t playTickTrack[] PROGMEM = {0x0045,0x168,0x0000};

#if(EXTENDED_NOTE_RANGE > 0)
//extended note range
#define NUM_PITCH 64
const uint8_t _halfPeriods[NUM_PITCH] PROGMEM= {246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33,31,29,28,26,25,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
#else
//regular note range
#define NUM_PITCH 36
const uint8_t _halfPeriods[NUM_PITCH] PROGMEM= {246,232,219,207,195,184,174,164,155,146,138,130,123,116,110,104,98,92,87,82,78,73,69,65,62,58,55,52,49,46,44,41,39,37,35,33};
#endif

#endif

void Sound::begin() {
#if(NUM_CHANNELS > 0)
	prescaler = 1;
	for(byte i=0; i<NUM_CHANNELS; i++){
		chanVolumes[i] = VOLUME_CHANNEL_MAX;
	}
	
	analogWrite(3, 1); //just to get the right register settings for PWM (hem)
	TCCR2B = (TCCR2B & B11111000) | 1; //set timer 2 prescaler to 1 -> 30kHz PWM on pin 3

	// initialize timer1 
	noInterrupts(); // disable all interrupts
	TCCR1A = 0;
	TCCR1B = 0;
	TCNT1 = 0;

	OCR1A = 280; // compare match register
	TCCR1B |= (1 << WGM12); // CTC mode
	TCCR1B |= (1 << CS10); // 1 prescaler
	TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
	interrupts(); // enable all interrupts

#endif
}

void Sound::playTrack(const uint16_t* track, const uint16_t** instruments, uint8_t channel){
#if(NUM_CHANNELS > 0)
	stopTrack(channel);
	trackData[channel] = (uint16_t*)track;
	trackInstruments[channel] = (uint16_t**)instruments;
	trackCursor[channel] = 0;
	trackPlaying[channel] = true;
	noteVolume[channel] = 9;
	//reinit commands
	volumeSlideStepDuration[channel] = 0;
	arpeggioStepDuration[channel] = 0;
	tremoloStepDuration[channel] = 0;
	//Serial.print("play track\n");
#endif
}

void Sound::playTrack(const uint16_t* track, uint8_t channel){
#if(NUM_CHANNELS > 0)
	playTrack(track, defaultInstruments, channel);
#endif
}

void Sound::updateTrack(){
#if(NUM_CHANNELS > 0)
	for (uint8_t i=0; i<NUM_CHANNELS; i++){
		if(trackPlaying[i]){
			if(noteDuration[i]==0){//if the end of the previous note is reached
				
				uint16_t data = pgm_read_word(trackCursor[i] + trackData[i]);
				
				if(data == 0){ //end of the track reached
					if(trackLooping[i] == true){
						trackCursor[i] = 0;
						data = pgm_read_word(trackCursor[i] + trackData[i]);
					}
					else{
						trackPlaying[i] = false;
						stopNote(i);
						//Serial.print("\ntrack end\n");
						continue;
					}
				}

				while (data & 0x0001){ //read all commands and instrument changes
					data >>= 2;
					//Serial.print("\ncmd\t");
					uint8_t command = data & 0x0F;
					data >>= 4;
					//Serial.print(command);
					//Serial.print("\t");
					//Serial.print(data & 0x1F);
					//Serial.print("\t");
					//Serial.println(int8_t(data >> 5) - 16);
					switch(command){
					case 0: //volume
						noteVolume[i] = data & 0x1F;
						break;
					case 1: //instrument
						instrumentData[i] = (uint16_t*)pgm_read_word(&(trackInstruments[i][data]));
						instrumentLength[i] = pgm_read_word(&(instrumentData[i][0])) & 0x00FF; //8 LSB
						instrumentLooping[i] = min((pgm_read_word(&(instrumentData[i][0])) >> 8), instrumentLength[i]); //8 MSB - check that the loop is shorter than the instrument length
						break;
					case 2: //volume slide
						volumeSlideStepDuration[i] = data & 0x1F;
						data >>= 5;
						volumeSlideStepSize[i] = data - 16;
						break;
					case 3:
						arpeggioStepDuration[i] = data & 0x1F;
						data >>= 5;
						arpeggioStepSize[i] = data - 16;
						break;
					case 4:
						tremoloStepDuration[i] = data & 0x1F;
						data >>= 5;
						tremoloStepSize[i] = data - 16;
						break;
					default:
						break;
					}
					trackCursor[i]++;
					data = pgm_read_word(trackCursor[i] + trackData[i]);
				}
				data >>= 2;

				uint8_t pitch = data & 0x003F;
				data >>= 6;
				
				uint8_t duration = data;
				
				//Serial.print("\ntrack update");
				//Serial.print("\t");
				//Serial.print(duration);
				//Serial.print("\t");
				//Serial.print(volume);
				//Serial.print("\t");
				//Serial.print(pitch);
				//Serial.print("\t");
				//Serial.print(instrumentID);
				//Serial.print("\n");
				
				//PLAY NOTE
				//set note
				notePitch[i] = pitch % NUM_PITCH; //limit to the number of available pitches
				noteDuration[i] = duration;
				//reinit vars
				instrumentNextChange[i] = 0;
				instrumentCursor[i] = 0;
				notePlaying[i] = true;
				_chanState[i] = true;
				commandsCounter[i] = 0;
				
				trackCursor[i]++;
			}
		}
	}
#endif
}

void Sound::stopTrack(uint8_t channel){
#if(NUM_CHANNELS > 0)
	stopNote(channel);
	trackPlaying[channel] = false;
#endif
}

void Sound::stopTrack(){
#if(NUM_CHANNELS > 0)
	for(uint8_t i=0; i<NUM_CHANNELS; i++){
		stopTrack(i);
	}
#endif
}

void Sound::stopNote(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
	return;
	notePlaying[channel] = false;
	//counters
	noteDuration[channel] = 0;
	instrumentCursor[channel] = 0;
	commandsCounter[channel] = 0;
	//output
	_chanOutput[channel] = 0;
	_chanOutputVolume[channel] = 0;
	_chanState[channel] = false;
	updateOutput();
#endif
}

void Sound::stopNote() {
#if(NUM_CHANNELS > 0)
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		stopNote(channel);
	}
#endif
}

void Sound::playNote(uint8_t pitch, uint8_t duration, uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
	return;
#endif
}

void Sound::updateNote() {
#if(NUM_CHANNELS > 0)
	for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
		if (notePlaying[i]) {
			
			if(noteDuration[i] == 0){
				stopNote(i);
				//Serial.println("note end");
				continue;
			} else {
				noteDuration[i]--;
			}
			
			if (instrumentNextChange[i] == 0) {
				//Serial.print("instr update:");
				//Serial.print("\t");
				
				//read the step data from the progmem and decode it
				uint16_t thisStep = pgm_read_word(&(instrumentData[i][1 + instrumentCursor[i]]));
				//Serial.print(thisStep, HEX);
				//Serial.print("\t");
				
				stepVolume[i] = thisStep & 0x0007;
				thisStep >>= 3;
				
				uint8_t stepNoise = thisStep & 0x0001;
				thisStep >>= 1;
				
				uint8_t stepDuration = thisStep & 0x003F;
				thisStep >>= 6;
				
				stepPitch[i] = thisStep;
				
				//apply the step settings
				instrumentNextChange[i] = stepDuration * prescaler;
				
				_chanNoise[i] = stepNoise;

				//Serial.print(stepPitch);
				//Serial.print("\t");
				//Serial.print(stepDuration);
				//Serial.print("\t");
				//Serial.print(stepNoise);
				//Serial.print("\t");
				//Serial.print(stepVolume);
				//Serial.print("\n");
				//Serial.print(_chanOutput[i]);
				//Serial.print("\n");
				
				instrumentCursor[i]++;
				
				if (instrumentCursor[i] >= instrumentLength[i]) {
					if (instrumentLooping[i]) {
						instrumentCursor[i] = instrumentLength[i] - instrumentLooping[i];
					} else {
						stopNote(i);
						//Serial.println("instrument end");
					}
				}
			}
			instrumentNextChange[i]--;
			
			commandsCounter[i]++;
			
			//UPDATE VALUES	
			//pitch
			int8_t pitch = notePitch[i] + stepPitch[i];
			if(arpeggioStepDuration[i])
			pitch += commandsCounter[i] / arpeggioStepDuration[i] * arpeggioStepSize[i];
			pitch = (pitch + NUM_PITCH) % NUM_PITCH; //wrap
			//volume
			int8_t volume = noteVolume[i];
			if(volumeSlideStepDuration[i])
			volume += commandsCounter[i] / volumeSlideStepDuration[i] * volumeSlideStepSize[i];
			if(tremoloStepDuration[i]){
				volume += ((commandsCounter[i]/tremoloStepDuration[i]) % 2) * tremoloStepSize[i];
			}
			volume = constrain(volume, 0, 9);
			
			noInterrupts();
			_chanHalfPeriod[i] = pgm_read_byte(_halfPeriods + pitch);
			_chanOutput[i] = _chanOutputVolume[i] = volume * globalVolume * chanVolumes[i] * stepVolume[i];
			//Serial.println(volume);
			interrupts();
		}
	}
#endif
}

void Sound::setChannelHalfPeriod(uint8_t channel, uint8_t halfPeriod) {
#if(NUM_CHANNELS > 0)
	_chanHalfPeriod[channel] = halfPeriod;
	_chanState[channel] = false;
	_chanCount[channel] = 0;
	updateOutput();
#endif
}

ISR(TIMER1_COMPA_vect){ // timer compare interrupt service routine
#if(NUM_CHANNELS > 0)
	Sound::generateOutput();
#endif
}

void Sound::generateOutput() {
#if(NUM_CHANNELS > 0)
	boolean outputChanged = false;
	//no for loop here, for the performance sake (this function runs 15 000 times per second...)
	//CHANNEL 0
	if (_chanOutputVolume[0]) {
		_chanCount[0]++;
		if (_chanCount[0] >= _chanHalfPeriod[0]) {
			outputChanged = true;
			_chanState[0] = !_chanState[0];
			_chanCount[0] = 0;
			if (_chanNoise[0]) {
				_rand = 67 * _rand + 71;
				_chanOutput[0] = _rand % _chanOutputVolume[0];
			}
		}
	}


	//CHANNEL 1
	#if (NUM_CHANNELS > 1)
	if (_chanOutputVolume[1]) {
		_chanCount[1]++;
		if (_chanCount[1] >= _chanHalfPeriod[1]) {
			outputChanged = true;
			_chanState[1] = !_chanState[1];
			_chanCount[1] = 0;
			if (_chanNoise[1]) {
				_rand = 67 * _rand + 71;
				_chanOutput[1] = _rand % _chanOutputVolume[1];
			}
		}
	}
	#endif

	//CHANNEL 2
	#if (NUM_CHANNELS > 2)
	if (_chanOutputVolume[2]) {
		_chanCount[2]++;
		if (_chanCount[2] >= _chanHalfPeriod[2]) {
			outputChanged = true;
			_chanState[2] = !_chanState[2];
			_chanCount[2] = 0;
			if (_chanNoise[2]) {
				_rand = 67 * _rand + 71;
				_chanOutput[2] = _rand % _chanOutputVolume[2];
			}
		}
	}
	#endif

	//CHANNEL 3
	#if (NUM_CHANNELS > 3)
	if (_chanOutputVolume[3]) {
		_chanCount[3]++;
		if (_chanCount[3] >= _chanHalfPeriod[3]) {
			outputChanged = true;
			_chanState[3] = !_chanState[3];
			_chanCount[3] = 0;
			if (_chanNoise[3]) {
				_rand = 67 * _rand + 71;
				_chanOutput[3] = _rand % _chanOutputVolume[3];
			}
		}
	}
	#endif

	if (outputChanged) {
		updateOutput();
	}
#endif
}

void Sound::updateOutput() {
#if(NUM_CHANNELS > 0)
	uint8_t output = 0;

	//CHANNEL 0
	if (_chanState[0]) {
		output += _chanOutput[0];
	}
	
	//CHANNEL 1
	#if (NUM_CHANNELS > 1)
	if (_chanState[1]) {
		output += _chanOutput[1];
	}
	#endif
	
	//CHANNEL 2
	#if (NUM_CHANNELS > 2)
	if (_chanState[2]) {
		output += _chanOutput[2];
	}
	#endif
	
	//CHANNEL 3
	#if (NUM_CHANNELS > 3)
	if (_chanState[3]) {
		output += _chanOutput[3];
	}
	#endif
	OCR2B = output; //60x faster than analogOutput() !
#endif
}

boolean Sound::isPlaying(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
	return false;
	if (notePlaying[channel] == true)
	return true;
	else
	return false;
#else
	return false;
#endif
}

void Sound::setTrackLooping(uint8_t channel, boolean loop) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
	return;
	trackLooping[channel] = loop;
#endif
}

void Sound::playOK(){
#if(NUM_CHANNELS > 0)
	playTrack(playOKTrack,0);
#endif
}

void Sound::playCancel(){
#if(NUM_CHANNELS > 0)
	playTrack(playCancelTrack,0);
#endif
}

void Sound::playTick(){
#if(NUM_CHANNELS > 0)
	playTrack(playTickTrack,0);
#endif
}

void Sound::setVolume(int8_t volume) {
#if NUM_CHANNELS > 0
	volume = (volume > volumeMax) ? volumeMax : volume;
	volume = (volume < 0) ? 0 : volume;
	globalVolume = volume;
#endif
}

uint8_t Sound::getVolume() {
#if NUM_CHANNELS > 0
	return globalVolume;
#else
	return 0;
#endif
}

void Sound::setVolume(int8_t volume, uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
	return;
	volume = (volume > VOLUME_CHANNEL_MAX) ? VOLUME_CHANNEL_MAX : volume;
	volume = (volume < 0) ? 0 : volume;
	chanVolumes[channel] = volume;
#endif
}

uint8_t Sound::getVolume(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
	return 255;
	return (chanVolumes[channel]);
#else
	return 0;
#endif
}
