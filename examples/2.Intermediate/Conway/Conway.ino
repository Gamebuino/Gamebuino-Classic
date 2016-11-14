#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
#include <EEPROM.h>
#include <avr/pgmspace.h>

//world size
//use power of 2 for better perfomance
#define SX 64
#define SY 32

byte buffer[SX*SY/8];
unsigned int pop = 0;
unsigned int prevPop = 0;
byte resetCounter = 0;
unsigned int gen = 0;
byte graphCursor = 0;

void setup() {
  gb.begin();
  gb.titleScreen(F("Conway's game of life"));
  gb.pickRandomSeed();
  reset();
}

void loop(){
  if(gb.update()){
    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Conway's game of life"));
      reset();
    }
    //compute next gen
    gen++;
    for (byte x = 0; x < SX; x++) {
      for (byte y = 0; y < SY; y++) {
        byte count = 0;
        if(getCell(x-1,          (y-1+SY)%SY)) count++;
        if(getCell(x,            (y-1+SY)%SY)) count++;
        if(getCell((x+1)%SX,     (y-1+SY)%SY)) count++;
        if(getCell((x+1)%SX,      y))   count++;
        if(getCell((x+1)%SX,     (y+1)%SY)) count++;
        if(getCell(x,            (y+1)%SY)) count++;
        if(getCell((x-1+SX)%SX,  (y+1)%SY)) count++;
        if(getCell((x-1+SX)%SX,   y))   count++;

        byte cell = getCell(x,y);


        if ( cell ) { //if there is a cell
          if ( (count < 2) || (count > 3) ) { //and more than 3 neighbors or less than 2
            gb.display.setColor(WHITE);
            gb.display.drawPixel(x+1,y+1);
            gb.display.setColor(BLACK,WHITE);
            pop--;
          }
        }
        else { //if there is no cell
          if (count == 3) { //and 3 neighbors
            gb.display.drawPixel(x+1,y+1);
            pop++;
          }
          else { //randomly add new cells if there is enough light
            if ((count==2) && (gb.backlight.ambientLight > 950) && !(random()%64)){
              gb.display.drawPixel(x+1,y+1);
              pop++;
            }
            else{
              if ((count==2) && (gb.backlight.ambientLight > 900) && !(random()%128)){
                gb.display.drawPixel(x+1,y+1);
                pop++;
              }
            }
          }
        }
      }
    }
    for (byte x = 0; x < SX; x++) {
      for (byte y = 0; y < SY; y++) {
        setCell(x, y, gb.display.getPixel(x+1, y+1));
      }
    }

    if(gb.frameCount%6){
      if(prevPop == pop){
        resetCounter++;
        if (resetCounter > 4){
          gb.display.setColor(BLACK,WHITE);
          gb.display.cursorX = 38;
          gb.display.cursorY = 36;
          gb.display.print("Gimme");
          gb.display.cursorX = 38;
          gb.display.cursorY = 36+gb.display.fontHeight;
          gb.display.print("light!");
        }
      }
      else{
        resetCounter = 0;
      }
      prevPop = pop;
    }
    gb.display.textWrap = false;

    gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
    gb.display.cursorY = 2;
    gb.display.println(F("Pop"));
    
    gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
    gb.display.print(pop);
    gb.display.println(" ");
    
    gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
    gb.display.cursorY += 2;
    gb.display.println(F("Gen"));
    
    gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
    gb.display.println(gen);
    
    gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
    gb.display.cursorY += 2;
    gb.display.println(F("Sun"));
    
    gb.display.cursorX = LCDWIDTH-4*gb.display.fontWidth;
    gb.display.print(F("\17")); //sun logo
    
    if(gb.backlight.ambientLight > 900)
      gb.display.print(F("\17"));
    else 
      gb.display.print(F(" "));
      
    if(gb.backlight.ambientLight > 950)
      gb.display.print(F("\17"));
    else 
      gb.display.print(F(" "));

    gb.display.setColor(WHITE);
    gb.display.drawFastVLine(graphCursor,34,14);
    gb.display.setColor(BLACK, WHITE);
    gb.display.drawFastVLine(graphCursor+1,34,14);
    gb.display.drawPixel(graphCursor, LCDHEIGHT-(pop/32)%16);
    graphCursor++;
    if(graphCursor > SX) graphCursor = 0;

  }
}

void setCell(byte x, byte y, boolean state){
  if (state)
    buffer[x + (y / 8) * SX] |= _BV(y % 8);
  else
    buffer[x + (y / 8) * SX] &= ~_BV(y % 8);
}

byte getCell(byte x, byte y){
  return (buffer[x + (y / 8) * SX] >> (y % 8)) & 0x1;
}

void reset(){
  gb.battery.show = false;
  gb.display.persistence = true;
  pop = 0;
  gen = 0;
  graphCursor = 0;
  gb.display.clear();
  gb.display.drawRect(0,0,SX+2,SY+2);
  for (byte x = 0; x < SX; x++) {
    for (byte y = 0; y < SY; y++) {
      if(!(random()%4)){
        setCell(x, y, true);
        gb.display.drawPixel(x+1,y+1);
        pop++;
      }
    }
    gb.display.update();
  }
}
