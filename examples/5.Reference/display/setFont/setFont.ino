#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

// these two default fonts are already defined in the Gamebuino library
// the "extern" modifier is used to be able to use a variable declared in another file
extern const byte font3x3[]; //a really tiny font
extern const byte font3x5[]; //a small but efficient font (default)
extern const byte font5x7[]; //a large, comfy font

void setup(){
  // initialize the Gamebuino object
  gb.begin();
  gb.display.setFont(font5x7);
  gb.titleScreen(F("Font size"));
}

void loop(){
  if(gb.update()){
    gb.display.setFont(font3x3);
    gb.display.println(F("The quick brown fox\njumps over the doge"));
    gb.display.setFont(font3x5);
    gb.display.print("3x5 font\n");
    gb.display.fontSize = 2;
    gb.display.print("3x5 font\n");
    gb.display.fontSize = 1;
      gb.display.setFont(font5x7);
    gb.display.print("5x7 font\n");
    gb.display.fontSize = 2;
    gb.display.print("5x7 font\n");
    gb.display.fontSize = 1;
    
    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Font size"));
    }
  }
}
