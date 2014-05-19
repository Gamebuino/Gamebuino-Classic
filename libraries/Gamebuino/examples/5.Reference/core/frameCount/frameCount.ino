#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin(F("Count example"));
}

void loop(){
  if(gb.update()){

    int count = gb.frameCount;
    gb.display.println(count);

    if((count%5) < 2){ //true for 2 frames once every 5 frames
      gb.display.println(F("BLINK"));
    }

  }
}
