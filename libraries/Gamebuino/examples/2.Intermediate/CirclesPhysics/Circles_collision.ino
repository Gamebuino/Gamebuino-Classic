#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

typedef struct{
  float x, y, vx, vy;
  int r;
} Circle;

void setup()
{
  gb.begin();
  initGame();
}

void loop()
{
  if(gb.update()){
    //pause the game if C is pressed
    if(gb.buttons.pressed(BTN_C)){
      initGame();
    }
    drawCircles();
    updateCircles();
    
  }
}

void initGame(){
  gb.titleScreen(F("Circle\ncollision\nphysics"));
  gb.pickRandomSeed(); //pick a different random seed each time for games to be different
  gb.battery.show = false; //hide the battery indicator
  initCircles();
}
