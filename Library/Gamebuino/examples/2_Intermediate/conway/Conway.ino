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
  gb.begin(F("Conway's game of life"));
  gb.battery.show = false;
  gb.display.persistence = true;
  randomSeed(analogRead(A7)*analogRead(A7)+analogRead(A7));
  reset();
}

void loop(){
  if(gb.update()){
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
            gb.display.drawPixel(x+(LCDWIDTH-SX)/2,y+1);
            gb.display.setColor(BLACK,WHITE);
            pop--;
          }
        } 
        else { //if there is no cell
          if (count == 3) { //and 3 neighbors
            gb.display.drawPixel(x+(LCDWIDTH-SX)/2,y+1);
            pop++;
          }
        }
      }
    }
    for (byte x = 0; x < SX; x++) { 
      for (byte y = 0; y < SY; y++) {
        setCell(x, y, gb.display.getPixel(x+(LCDWIDTH-SX)/2, y+1));
      }
    }

    if(gb.getFrameCount()%6){
      if(prevPop == pop){
        resetCounter++;
        if (resetCounter == 16){
          gb.sound.playCancel();
          gb.display.setColor(WHITE);
          gb.display.fillRect(32,LCDHEIGHT-14, LCDWIDTH-32, 14);
          gb.display.setColor(BLACK,WHITE);
          gb.display.setCursor(38,38);
          gb.display.print("\25:restart");
          while(1){
            if(gb.update()){
              if(gb.buttons.pressed(BTN_A)){
                reset();
                break;
              }
            }
          }
        }
      }
      else{
        resetCounter = 0;
      }
      prevPop = pop;
    }

    gb.display.setCursor(0,LCDHEIGHT-2*FONTHEIGHT);
    gb.display.print(F("Pop:"));
    gb.display.print(pop);
    gb.display.print(F(" \nGen:"));
    gb.display.print(gen);
    gb.display.print(" ");
    
    gb.display.setColor(WHITE);
    gb.display.drawFastVLine(32+graphCursor,34,14);
    gb.display.setColor(BLACK, WHITE);
    gb.display.drawFastVLine(32+graphCursor+1,34,14);
    gb.display.drawPixel(32+graphCursor, LCDHEIGHT-pop/32);
    graphCursor++;
    if(graphCursor > LCDWIDTH-32) graphCursor = 0;

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
  pop = 0;
  gen = 0;
  graphCursor = 0;
  gb.display.clear();
  gb.display.drawRect((LCDWIDTH-SX)/2-1,0,SX+2,SY+2);
  for (byte x = 0; x < SX; x++) { 
    for (byte y = 0; y < SY; y++) {
      if(!(random()%4)){
        setCell(x, y, true);
        gb.display.drawPixel(x+(LCDWIDTH-SX)/2,y+1);
        pop++;
      }
    }
    gb.display.update();
  }
}







