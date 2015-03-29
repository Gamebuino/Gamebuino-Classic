#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

//bitmap encoded using the Bitmap Encoder which can be found here : http://gamebuino.com/wiki/index.php?title=Download
const byte pointerBitmap[] PROGMEM = {16,22,0xC,0x0,0x12,0x0,0x12,0x0,0x12,0x0,0x12,0x0,0x13,0x80,0x12,0x70,0x12,0x4C,0x12,0x4A,0xD2,0x49,0xB0,0x9,0x90,0x1,0x90,0x1,0x40,0x1,0x40,0x1,0x20,0x1,0x20,0x2,0x10,0x2,0x10,0x2,0x8,0x4,0x8,0x4,0xF,0xFC,};

void setup(){
  gb.begin();
  gb.titleScreen(F("Color example"));
  gb.setFrameRate(42); //set the frequency at 42 frames per second for better results with gray
}

void loop(){
  if(gb.update()){
    //set the color to be use to black. If there is no second color specified, the background will be transparent
    gb.display.setColor(BLACK);
    gb.display.fillRect(34,0, 16, 48);
    
    //rhe colors available are :
    //  WHITE
    //  BLACK
    //  INVERT : negative effect
    //  GRAY : creates gray using screen's persistence by flickering the pixels quickly
    // experiments have shown that running the screen at 42 frames per second gives better results with gray
    
    //the color value can be stored in a variable :
    byte color = BLACK;
    //when the background and the foreground colors are the same, the background is left transparent.
    byte background = WHITE;
    
    //then you can change the color depending on conditions
    //here we use the buttons :
    if(gb.buttons.repeat(BTN_RIGHT,1)){
      color = WHITE;
      gb.display.println(F("WHITE"));
    }
    if(gb.buttons.repeat(BTN_LEFT,1)){
      color = INVERT;
      gb.display.println(F("INVERT"));
    }
    if(gb.buttons.repeat(BTN_DOWN,1)){
      color = GRAY;
      gb.display.println(F("GRAY"));
    }
    
    //change if background color if buttons are pressed :
    if(gb.buttons.repeat(BTN_A,1)){
      background = BLACK;
      gb.display.println(F("BLACK"));
    }
    if(gb.buttons.repeat(BTN_B,1)){
      background = INVERT;
      gb.display.println(F("INVERT"));
    }
    
    //set the color define above
    gb.display.setColor(color, background);
    
    //then draw a few things :
    //a rectangle
    gb.display.fillRect(20,16,16,16);
    //a bitmap
    gb.display.drawBitmap(40, 16, pointerBitmap);
    //some text
    gb.display.cursorX = 40;
    gb.display.cursorY = 0;
    gb.display.fontSize = 2;
    gb.display.print(F("Text"));
    gb.display.fontSize = 1;
    
    
    //don't forget to allow the user to get back to the title screen when C is used :
    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Bitmap example"));
    }
  }
}
