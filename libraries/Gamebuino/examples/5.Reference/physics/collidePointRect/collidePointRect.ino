#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

byte player_x = 20;
byte player_y = 20;

byte oponent_x = LCDWIDTH/2;
byte oponent_y = LCDHEIGHT/2;
byte oponent_w = 17;
byte oponent_h = 12;

void setup(){
  gb.begin();
  gb.titleScreen(F("collidePointRect"));
}

void loop(){
  if(gb.update()){
    gb.display.println(F("\n  Move with arrows"));

    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("collidePointRect"));
    }

    //update player
    if(gb.buttons.repeat(BTN_RIGHT,1)){
      player_x = player_x + 1;
    }
    if(gb.buttons.repeat(BTN_LEFT,1)){
      player_x = player_x - 1;
    }
    if(gb.buttons.repeat(BTN_DOWN,1)){
      player_y = player_y + 1;
    }
    if(gb.buttons.repeat(BTN_UP,1)){
      player_y = player_y - 1;
    }

    //update collisions
    if(gb.collidePointRect(player_x, player_y, oponent_x, oponent_y, oponent_w, oponent_h)){
      gb.sound.playTick();
      gb.display.print(F("  Colliding"));
    }

    //draw player
    gb.display.drawPixel(player_x, player_y);
    
    //draw a few lines to help seeing the pixel:
    gb.display.drawLine(0, player_y, 3, player_y);
    gb.display.drawLine(player_x, 0, player_x, 3);
    gb.display.drawLine(player_x,LCDHEIGHT-3, player_x, LCDHEIGHT);
    gb.display.drawLine(LCDWIDTH - 3, player_y, LCDWIDTH, player_y);

    //draw oponent
    gb.display. drawRect(oponent_x, oponent_y, oponent_w, oponent_h);
  }
}


