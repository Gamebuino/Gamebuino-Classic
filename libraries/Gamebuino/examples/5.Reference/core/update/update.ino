#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

void setup(){
  gb.begin(F("Example game"));

  while(1){ //loops forever
    if(gb.update()){ //update everything
    
      //put your game here. I will run at a fixed frequency (20 times per second by default)

      if(gb.buttons.pressed(BTN_C)) //break out of the loop if C is pressed
        break;
    }
  }
}

void loop(){

}
