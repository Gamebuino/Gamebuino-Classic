#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin(F("Popup example")); 
}

void loop(){
  if(gb.update()){
    if(gb.buttons.pressed(BTN_A))
      gb.popup(F("A pressed"),20);
    if(gb.buttons.pressed(BTN_B))
      gb.popup(F("B pressed"),20);
    if(gb.buttons.pressed(BTN_C))
      gb.popup(F("C pressed"),20);
  }
}
