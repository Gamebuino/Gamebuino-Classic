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
uint8_t _chanCount[NUM_CHANNELS];
boolean _chanState[NUM_CHANNELS];
uint8_t _chanHalfPeriod[NUM_CHANNELS];
uint8_t _chanOutputVolume[NUM_CHANNELS];
uint8_t _chanOutput[NUM_CHANNELS];
boolean _chanNoise[NUM_CHANNELS];

#define NUM_FREQ 50
uint8_t _halfPeriods[NUM_FREQ] = {219, 207, 195, 184, 174, 164, 155, 147, 138, 131, 123, 116, 110, 104, 98, 92, 87, 82, 78, 74, 69, 66, 62, 58, 55, 52, 49, 46, 44, 41, 39, 37, 35, 33, 31, 29,};

PROGMEM uint16_t sound_OK[] = {0x301E, 0x601E, 0x0000};
PROGMEM uint16_t sound_Cancel[] = {0x601E, 0x301E, 0x0000};
PROGMEM uint16_t sound_Tick[] = {0x601F, 0x0000};
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

void Sound::play(uint16_t* soundData, uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
    //stop(channel);
    data[channel] = soundData;
    playing[channel] = true;
    nextChange[channel] = 0;
    cursorPosition[channel] = 0;
    _chanState[channel] = true;
#endif
}

boolean Sound::isPlaying(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return false;
    if (playing[channel] == true)
        return true;
    else
        return false;
#endif
}

void Sound::stop(uint8_t channel) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
    playing[channel] = false;
    _chanOutput[channel] = 0;
    _chanOutputVolume[channel] = 0;
    _chanState[channel] = false;
    updateOutput();
#endif
}

void Sound::stop() {
#if(NUM_CHANNELS > 0)
    for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
        stop(channel);
    }
#endif
}

void Sound::setLooping(uint8_t channel, boolean loop) {
#if(NUM_CHANNELS > 0)
	if(channel>=NUM_CHANNELS)
		return;
    looping[channel] = loop;
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

void Sound::playOK() {
#if(NUM_CHANNELS > 0)
    play(sound_OK, 0);
#endif
}

void Sound::playCancel() {
#if(NUM_CHANNELS > 0)
    play(sound_Cancel, 0);
#endif
}

void Sound::playTick() {
#if(NUM_CHANNELS > 0)
    play(sound_Tick, 0);
#endif
}

void Sound::update() {
#if(NUM_CHANNELS > 0)
    noInterrupts();
    for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
        if (playing[i]) {
            if (nextChange[i] == 0) {
                uint16_t thisNote = pgm_read_word(data[i] + cursorPosition[i]);

                if (thisNote == 0) {
                    if (looping[i]) {
                        cursorPosition[i] = 0;
                        nextChange[i] = 1;
                    } else {
                        stop(i);
                    }
                    continue;
                }

                uint8_t noteNoise = thisNote & 0x0001;
                thisNote >>= 1;
                uint8_t noteVolume = (thisNote & 0x0007);
                thisNote >>= 3;
                uint8_t noteDuration = thisNote & 0x003F;
                thisNote >>= 6;
                uint8_t noteID = thisNote & 0x003F;
                uint8_t noteHalfPeriod = _halfPeriods[noteID];

                _chanNoise[i] = noteNoise;
                _chanOutputVolume[i] = globalVolume * chanVolumes[i] * noteVolume;
                _chanOutput[i] = _chanOutputVolume[i];
                nextChange[i] = noteDuration*prescaler;
                _chanHalfPeriod[i] = noteHalfPeriod;

                //                Serial.print(i);
                //                Serial.print(" ");
                //                Serial.print(noteHalfPeriod);
                //                Serial.print(" ");
                //                Serial.print(noteDuration);
                //                Serial.print(" ");
                //                Serial.print(noteVolume);
                //                Serial.print(" ");
                //                Serial.print(_chanOutputVolume[i]);
                //                Serial.print(" ");
                //                Serial.println(noteNoise);

                cursorPosition[i]++;
            }
            nextChange[i]--;
        }
    }
    interrupts();
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

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
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