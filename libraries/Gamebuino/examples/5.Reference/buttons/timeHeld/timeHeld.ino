#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin(F("timeHeld example"));
}

void loop(){
  if(gb.update()){
    gb.display.print("gb.buttons.timeHeld\n\n");

    gb.display.print("BTN_A:");
    gb.display.println(gb.buttons.timeHeld(BTN_A));

    gb.display.print("BTN_B:");
    gb.display.println(gb.buttons.timeHeld(BTN_B));

    gb.display.print("BTN_C:");
    gb.display.println(gb.buttons.timeHeld(BTN_C));
  }
}

