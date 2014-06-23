#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

// these two default fonts are already defined in the Gamebuino library
// the "extern" modifier is used to be able to use a variable declared in another file
extern const byte font3x5[];
extern const byte font5x7[];

void setup(){
  // initialize the Gamebuino object
  gb.begin();
  gb.display.setFont(font5x7);
  gb.titleScreen(F("Font size"));
}

void loop(){
  if(gb.update()){
      gb.display.setFont(font3x5);
    gb.display.print("3x5 font\n");
    gb.display.setTextSize(2);
    gb.display.print("3x5 font\n");
    gb.display.setTextSize(1);
      gb.display.setFont(font5x7);
    gb.display.print("\n5x7 font\n");
    gb.display.setTextSize(2);
    gb.display.print("5x7 font\n");
    gb.display.setTextSize(1);
    
    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Font size"));
    }
  }
}
