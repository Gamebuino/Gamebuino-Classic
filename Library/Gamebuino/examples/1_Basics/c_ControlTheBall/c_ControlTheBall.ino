//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//importe the Gamebuino library
#include <Gamebuino.h>
//creates a Gamebuino object named gb
Gamebuino gb;

//declare all the variables needed for the game :
int ball_x = LCDWIDTH/2; //set the horizontal position to the middle of the screen
int ball_y = LCDHEIGHT/2; //vertical position
int ball_vx = 1; //horizontal velocity
int ball_vy = 1; //vertical velocity
int ball_size = 6; //the size of the ball in number of pixels

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin(F("Controls"));
}

// the loop routine runs over and over again forever
void loop(){
  //updates the gamebuino (the display, the sound, the auto backlight... everything)
  //returns true when it's time to render a new frame (20 times/second)
  if(gb.update()){
    
    //move the ball using the buttons
    if(gb.buttons.repeat(BTN_RIGHT,2)){ //every 2 frames when the right button is held down
      ball_x = ball_x + ball_vx; //increase the horizontal position by the ball's velocity
      gb.sound.playTick(); //play a preset "tick" sound every time the button is pressed
    }
    if(gb.buttons.repeat(BTN_LEFT,2)){
      ball_x = ball_x - ball_vx;
      gb.sound.playTick();
    }
    if(gb.buttons.repeat(BTN_DOWN,2)){
      ball_y = ball_y + ball_vy;
      gb.sound.playTick();
    }
    if(gb.buttons.repeat(BTN_UP,2)){
      ball_y = ball_y - ball_vy;
      gb.sound.playTick();
    }
    //bonus : play a preset sound when A and B are pressed
    if(gb.buttons.pressed(BTN_A)){
      gb.sound.playOK();
    }
    if(gb.buttons.pressed(BTN_B)){
      gb.sound.playCancel();
    }
    
    //check that the ball is not going out of the screen
    //if the ball is touching the left side of the screen
    if(ball_x < 0){
      //bring it back in the screen
      ball_x = 0;
    }
    //if the ball is touching the right side
    if((ball_x + ball_size) > LCDWIDTH){
      ball_x = LCDWIDTH - ball_size;
    }
    //if the ball is touching the top side
    if(ball_y < 0){
      ball_y = 0;
    }
    //if the ball is touching the down side
    if((ball_y + ball_size) > LCDHEIGHT){
      ball_y = LCDHEIGHT - ball_size;
    }
    
    //draw the ball on the screen
    gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
    
  }
}

