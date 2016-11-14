#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin();
  gb.titleScreen(F("Buttons example"));
  gb.setFrameRate(10); //lower the FPS to have time to see what happens
}

void loop(){
  if(gb.update()){
    gb.display.println("- BUTTONS -");

    gb.display.print("Pressed: ");
    if(gb.buttons.pressed(BTN_A)) gb.display.print("\25");
    else gb.display.print(" ");
    if(gb.buttons.pressed(BTN_B)) gb.display.print("\26");
    else gb.display.print(" ");
    gb.display.println();

    gb.display.print("Released:");
    if(gb.buttons.released(BTN_A)) gb.display.print("\25");
    else gb.display.print(" ");
    if(gb.buttons.released(BTN_B)) gb.display.print("\26");
    else gb.display.print(" ");
    gb.display.println();

    gb.display.print("Held:    ");
    if(gb.buttons.held(BTN_A,10)) gb.display.print("\25");
    else gb.display.print(" ");
    if(gb.buttons.held(BTN_B,10)) gb.display.print("\26");
    else gb.display.print(" ");
    gb.display.println();

    gb.display.print("Repeat:  ");
    if(gb.buttons.repeat(BTN_A,10)) gb.display.print("\25");
    else gb.display.print(" ");
    if(gb.buttons.repeat(BTN_B,10)) gb.display.print("\26");
    else gb.display.print(" ");
    gb.display.println();

    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Buttons example"));
    }
  }
}


