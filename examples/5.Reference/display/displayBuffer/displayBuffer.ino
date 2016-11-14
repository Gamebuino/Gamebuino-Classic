#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

uint8_t *displayBuffer; //pointer to store the buffer's address
byte i = 0;

void setup(){
  gb.begin();
  gb.titleScreen(F("Direct display\nbuffer access"));
  //get the buffer's address and assign it to displayBuffer
  displayBuffer = gb.display.getBuffer();
}

void loop(){
  if(gb.update()){
    //directly access the display's buffer value
    displayBuffer[i] = BLACK;
    i++;
  }
}
