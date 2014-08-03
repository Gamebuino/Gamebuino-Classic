#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb = Gamebuino();

//define de Box structure for obstacles
typedef struct {
  byte w;
  byte x;
  byte h;
  byte y;
} Box;

//define de MovingBox structure for player
typedef struct {
  byte w;
  float x;
  float xv;
  byte h;
  float y;
  float yv;
} MovingBox;

void setup()
{
  gb.begin();
  initGame();
}

void loop(){
  if(gb.update()){
    //pause the game if C is pressed
    if(gb.buttons.pressed(BTN_C)){
      initGame();
    }
    
    updatePlayer();
    updateObstacles();
    
    drawObstacles();
    drawPlayer();
    //gb.display.print(F("CPU:"));
    //gb.display.print(gb.getCpuLoad());
    //gb.display.print(F("%"));
    
  }
}

void initGame(){
  gb.titleScreen(F("    PHYSICS DEMO\n\nControls:\n \25 jump\n \26 crouch "));
  gb.pickRandomSeed(); //pick a different random seed each time for games to be different
  gb.battery.show = false; //hide the battery indicator
  
  initObstacles();
  initPlayer();
}

