#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin();
  gb.titleScreen(F("timeHeld example"));
}

void loop(){
  if(gb.update()){
    gb.display.print("gb.buttons.timeHeld\n\n");

    gb.display.print("BTN_A:");
    gb.display.println(gb.buttons.timeHeld(BTN_A));

    gb.display.print("BTN_B:");
    gb.display.println(gb.buttons.timeHeld(BTN_B));

    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("timeHeld example"));
    }
  }
}


