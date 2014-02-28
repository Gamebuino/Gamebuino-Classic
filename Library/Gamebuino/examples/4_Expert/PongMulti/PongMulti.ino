#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb = Gamebuino();

#include <Wire.h>

boolean isMaster;
boolean paused = false;

//I2C variables identifiers :
#define PLAYER_Y 10
#define BALL_X 20
#define BALL_Y 21

//we use char type instead of int because it's easier to send over I2C
char player_h = 12;
char player_w = 2;
char player_y = (LCDHEIGHT-player_h)/2;
char oponent_h = 12;
char oponent_w = 2;
char oponent_y = (LCDHEIGHT-oponent_h)/2;
char ball_size = 4;
char ball_x = (LCDWIDTH-ball_size)/2;
char ball_y = (LCDHEIGHT-ball_size)/2;
char ball_vx = 1;
char ball_vy = 1;

#define MENULENGTH 2
char* PROGMEM menu[MENULENGTH] = {
  "Master (host)",
  "Slave (join)"
};

///////////////////////////////////// SETUP
void setup() {
  gb.begin();
  Wire.begin();
  Serial.begin(9600);
}

///////////////////////////////////// LOOP
void loop() {
  switch(gb.menu(menu, MENULENGTH)){
  case 0: //Host
    setupMaster();
    break;
  case 1: //Join
    setupSlave();
    break;
  default:
    break;
  }
  play();
}



