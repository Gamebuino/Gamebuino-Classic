#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin();
  gb.titleScreen(F("Popup example"));
}

void loop(){
  if(gb.update()){
    gb.display.println("Press \25 \26 or \27\nto see a popup");
    if(gb.buttons.pressed(BTN_A))
      gb.popup(F("A pressed"),5);
    if(gb.buttons.pressed(BTN_B))
      gb.popup(F("B pressed"),5);
    if(gb.buttons.pressed(BTN_C))
      gb.popup(F("C pressed"),5);
  }
  
  if(gb.buttons.pressed(BTN_C)){
    gb.titleScreen(F("Popup example"));
  }
}

