#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

char text[13] = "Default text";

void setup(){
  gb.begin();
  gb.startMenu(F("Keyword example"));
  gb.keyboard(text, 12);
}

void loop(){
  if(gb.update()){
    gb.display.println(F("You wrote:"));
    gb.display.println(text);

    if(gb.buttons.pressed(BTN_C)){
      gb.startMenu(F("Keyword example"));
      gb.keyboard(text, 12);
    }
  }
}

