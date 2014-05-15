#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb = Gamebuino();

#include <Wire.h>

boolean isMaster;
boolean paused = true;
boolean disconnected = false;
boolean slave_updated = false;


//I2C variables identifiers :
#define SLAVE_PAUSED 1
#define I_AM_MASTER 2
#define PLAYER_Y 10
#define BALL_X 20
#define BALL_Y 21
#define PLAYER_SCORE 30
#define OPONENT_SCORE 31
#define SLAVE_DATA_BUFFER_LENGTH 2

//we use char type instead of int because it's easier to send over I2C
//player variables
char player_w = 3;
char player_h = 16;
char player_y = (LCDHEIGHT-player_h)/2;
char player_x = 0;
char player_vy = 2;
byte player_score = 0;
//oponent variables
char oponent_w = 3;
char oponent_h = 16;
char oponent_x = LCDWIDTH - oponent_w;
char oponent_y = (LCDHEIGHT-oponent_h)/2;
char oponent_vy = player_vy;
byte oponent_score = 0;
//ball variables
char ball_size = 6;
char ball_x = (LCDWIDTH-ball_size)/2;
char ball_y = (LCDHEIGHT-ball_size)/2;
char ball_vx = 3;
char ball_vy = 3;

#define MENULENGTH 2
char* PROGMEM menu[MENULENGTH] = {
  "Host (master)",
  "Join (slave)"
};

///////////////////////////////////// SETUP
void setup() {
  gb.begin(F("Pong Multiplayer")); //initialize the Gamebuino
  gb.battery.show = false; //hide the battery indicator
  //can be either master or slave:
  setupMaster();
  setupSlave();
}

///////////////////////////////////// LOOP
void loop() {

  paused = true;
  isMaster = false;
  player_score = 0;
  oponent_score = 0;
  
  switch(gb.menu(menu, MENULENGTH)){
  case 0: //Host
    paused = false;
    disconnected = false;
    isMaster = true;
    break;
  case 1: //Join
    paused = false;
    disconnected = false;
    isMaster = false;
    break;
  default:
    break;
  }

  while(1){ //infinite loop, like the loop() function
    if(gb.update()){ //update the Gamebuino
      if(gb.buttons.pressed(BTN_C)){
        break; //break the infinite loop to get back to the menu
      }
      if(!paused){
        if(gb.buttons.repeat(BTN_UP, 1)){
          player_y = max(0, player_y - player_vy);
        }
        if(gb.buttons.repeat(BTN_DOWN, 1)){
          player_y = min(LCDHEIGHT-player_h, player_y + player_vy);
        }
      }

      gb.display.setTextSize(1);
      if(isMaster){
        gb.display.print(F(" master "));
        updateMaster();
      }
      else {
        gb.display.print(F(" slave "));
        updateSlave();
      }

      updateDisplay();
    }
  }
}












