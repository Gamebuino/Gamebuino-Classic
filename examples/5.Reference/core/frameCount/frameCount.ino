#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin();
  gb.titleScreen(F("Count example"));
}

void loop(){
  if(gb.update()){
    int count = gb.frameCount;
    gb.display.println(count);

    if( (count / 8) % 2 ){ //true half of the time on 8 frames
      gb.display.println(F("BLINK"));
    }

    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Count example"));
    }
  }
}
