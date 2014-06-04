#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

PROGMEM uint16_t myTrack[] = {
0x0005,                0xA00,0xA10,0xA20,0x0045,0xA00,0xA10,0xA20,
0x0005, 0x0001, 0x8849,0xA00,0xA10,0xA20,0x0045,0xA00,0xA10,0xA20,
0x0005, 0x8241, 0x7849,0xA00,0xA10,0xA20,0x0045,0xA00,0xA10,0xA20,
0x0};

///////////////////////////////////// SETUP
void setup() {
  gb.begin(F("Music"));
  gb.sound.playTrack(myTrack,0);
}

void loop(){
  if(gb.update()){
    if(gb.buttons.pressed(BTN_A)){
      gb.sound.playOK();
    }
    if(gb.buttons.pressed(BTN_B)){
      gb.sound.playCancel();
    }

  }
}

