#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;



byte player_x = 10;
byte player_y = 10;
byte player_w = 8;
byte player_h = 16;

byte oponent_x = LCDWIDTH/2;
byte oponent_y = LCDHEIGHT/2;
byte oponent_w = 17;
byte oponent_h = 12;

void setup(){
  gb.begin();
  gb.titleScreen(F("collideRectRect"));
}


void loop(){
  if(gb.update()){
    gb.display.println(F("Move with arrows"));

    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("collideRectRect"));
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

    //update collision
    if(gb.collideRectRect(player_x, player_y, player_w, player_h, oponent_x, oponent_y, oponent_w, oponent_h)){
      gb.sound.playTick();
      gb.display.print(F("Collliding"));
    }

    //draw rectangles
    gb.display.setColor(INVERT);
    gb.display.fillRect(player_x, player_y, player_w, player_h);
    gb.display.fillRect(oponent_x, oponent_y, oponent_w, oponent_h);
    gb.display.setColor(BLACK);
  }
}

