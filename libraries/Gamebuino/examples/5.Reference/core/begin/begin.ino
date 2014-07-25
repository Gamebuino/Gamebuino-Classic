#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

static unsigned char PROGMEM logo[]=
{
  8,8,
  B00000000,
  B00100100,
  B00011000,
  B01111110,
  B00111100,
  B01111110,
  B00111100,
  B00000000,
};

void setup(){
  // initialize the Gamebuino object
  gb.begin();
  gb.titleScreen(F("Example game"), logo);
}

void loop(){
  if(gb.update()){
    gb.display.println("Hello world");
    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Example game"), logo);
    }
  }
}
