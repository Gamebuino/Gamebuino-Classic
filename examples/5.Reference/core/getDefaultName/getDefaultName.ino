#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

char name[11];

void setup(){
  gb.begin();
  gb.titleScreen(F("Name example"));
  gb.getDefaultName(name);
}

void loop(){
  if(gb.update()){
    gb.display.print("Hello ");
    gb.display.print(name);
  }

  if(gb.buttons.pressed(BTN_C)){
    gb.titleScreen(F("Name example"));
  }
}
