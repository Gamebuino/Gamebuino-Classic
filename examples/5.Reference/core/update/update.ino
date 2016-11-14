#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin();
  gb.titleScreen(F("Update example"));
}

void loop(){
  while(1){ //loops forever
    if(gb.update()){ //update everything

      //put your game here. I will run at a fixed frequency (20 times per second by default)
      gb.display.println(F("First loop"));
      gb.display.println(F("Press \25 to continue"));

      if(gb.buttons.pressed(BTN_A)) //break out of the loop if C is pressed
        break;

      if(gb.buttons.pressed(BTN_C)){
        gb.titleScreen(F("Update example"));
      }
    }
  }

  while(1){ //loops forever
    if(gb.update()){ //update everything

      //put your game here. I will run at a fixed frequency (20 times per second by default)
      gb.display.println(F("Second loop"));
      gb.display.println(F("Press \25 to continue"));

      if(gb.buttons.pressed(BTN_A)) //break out of the loop if C is pressed
        break;

      if(gb.buttons.pressed(BTN_C)){
        gb.titleScreen(F("Update example"));
      }
    }
  }
}
