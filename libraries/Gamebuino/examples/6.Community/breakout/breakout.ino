#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb = Gamebuino();
static unsigned char PROGMEM logo[] =
{
  64, 28,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00001111, B11100000, B00000000, B00000000, B01100000, B00000000, B00000001, B00000000,
  B00001111, B11100000, B00000000, B00000000, B01100000, B00000000, B00000011, B00000000,
  B00001100, B00100011, B01001110, B00011100, B01100110, B01110001, B10111111, B11000000,
  B00001111, B11100011, B10111011, B00010110, B01101100, B11111101, B10111011, B11000000,
  B00001111, B11100011, B00111110, B00001110, B01111000, B11011101, B10111011, B00000000,
  B00001100, B00100011, B00111000, B00110110, B01111100, B11011101, B10111011, B00000000,
  B00001111, B11100011, B00011110, B00111110, B01101100, B01111001, B11111011, B11000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B11111100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000001, B11111110, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000001, B11111110, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000100, B11111110, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000001, B11111100, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00010000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00111111, B11111111, B11111111, B11110000, B00000000, B00000000,
  B00000000, B00000000, B11111111, B11111111, B11111111, B11111110, B00000000, B00000000,
  B00000000, B00000001, B11111111, B11111111, B11111111, B11111110, B00000000, B00000000,
  B00000000, B00000001, B11111111, B11111111, B11111111, B11111110, B00000000, B00000000,
};

boolean paused = false;
#define BrickW 5
#define BrickH 2
#define BrickSpaceX 1
#define BrickSpaceY 1
#define BrickColsMax 11
#define BrickRows (LCDWIDTH/(BrickSpaceX+BrickW))-1
int BrickCols = 4;

static unsigned char PROGMEM explode0[] = {
  8, 8,
  B00010000,
  B01000001,
  B00000100,
  B10010000,
  B00000100,
  B01000001,
  B00001000,
  B10100100,
};

static unsigned char PROGMEM explode1[] = {
  8, 8,
  B00000000,
  B01000001,
  B00000100,
  B00010000,
  B00000100,
  B01000000,
  B00001000,
  B00000000,
};

static unsigned char PROGMEM explode2[] = {
  8, 8,
  B00000000,
  B00000000,
  B00000000,
  B00010000,
  B00000100,
  B01000000,
  B00001000,
  B00000000,
};


typedef struct
{
  int x;
  int y;
  int vx;
  int vy;
  byte w;
  byte h;
  int score;
}  PlayerStruct;
PlayerStruct Player = (PlayerStruct) {
  0, 0, 0, 0, 0, 0, 0
};

typedef struct
{
  int x;
  int y;
  int vx;
  int vy;
  int v;
  byte w;
  byte h;
  byte size;
  boolean exist;
}  BallStruct;
BallStruct Ball = (BallStruct) {
  0, 0, 0, 0, 0, 0, 0, true
};

typedef struct
{
  byte x;
  byte y;
  byte w;
  byte h;
  boolean exist;
}  BrickStruct;


///////////////////////////////////// SETUP
void setup() {
  randomSeed(gb.battery.voltage + gb.backlight.ambientLight);
  gb.begin();
  gb.titleScreen(F("Breakout by Ripper121"), logo);
  gb.battery.show = false; //hide the battery indicator
}

BrickStruct Brick[BrickRows][BrickColsMax];
int BrickCount = 0;
int DestBrickCount = 0;
boolean GameReset = true;
boolean GameWin = false;
boolean GameFirstRun = true;
byte Level = 1;

///////////////////////////////////// LOOP
void loop() {
  //Game Init/NextLevel
  if (GameReset) {
    if (GameWin) {
      gb.display.clear();
      gb.display.fontSize = 1;
      Level++;
      if (BrickCols < BrickColsMax) {
        BrickCols++;
        gb.display.print("Next level!\n");
        gb.display.print("Level:\n");
        gb.display.print(Level);
        gb.display.print("\nScore:\n");
        gb.display.print(Player.score);
      }
      else
      {
        gb.display.print("You win the game!\n");
        gb.display.print("Level:\n");
        gb.display.print(Level);
        gb.display.print("\nScore:\n");
        gb.display.print(Player.score);
      }
      gb.display.update();
      gb.sound.playOK();
      delay(5000);
    }
    else if (!GameFirstRun)
    {
      gb.display.clear();
      gb.display.fontSize = 1;
      gb.display.print("Game Over!\n");
      gb.display.print("Level:\n");
      gb.display.print(Level);
      gb.display.print("\nScore:\n");
      gb.display.print(Player.score);
      gb.sound.playCancel();
      gb.display.update();
      BrickCols = 4;
      Player.score = 0;
      Level = 0;
      DestBrickCount = 0;
      delay(5000);
    }

    //Brick Init
    BrickCount = 0;
    for (byte row = 0; row < BrickRows; row++)
      for (byte col = 0; col < BrickCols; col++) {
        Brick[row][col] = (BrickStruct) {
          (LCDWIDTH / (BrickSpaceX + BrickW)) / 4 + row * (BrickW + BrickSpaceX), 5 + BrickSpaceY + col * (BrickH + BrickSpaceY), BrickW, BrickH, true
        };
        BrickCount++;
      }

    GameReset = false;
    GameWin = false;
    GameFirstRun = false;

    //Player Init
    Player.w = 10;
    Player.h = 2;
    Player.x = (LCDWIDTH - Player.w) / 2;
    Player.vx = 2;
    Player.y = LCDHEIGHT - Player.h;
    Player.vy = 0;



    //Ball Init
    Ball.size = 3;
    Ball.x = (LCDWIDTH - Player.w) / 2;
    Ball.y = LCDHEIGHT - Player.h - Ball.size;
    Ball.v = 1;
    Ball.vx = -Ball.v;
    Ball.vy = -Ball.v;
    //Random move the ball left or right    
    if (random(0, 2) == 1) {
      Ball.vx = -Ball.vx;
    }
  }

  //Game Run
  if (gb.update()) {

    //pause the game if C is pressed
    if (gb.buttons.pressed(BTN_C)) {
      paused = !paused; //toggle paused state
    }
    if (!paused) {

      //move the player
      if (gb.buttons.repeat(BTN_LEFT, 1)) {
        Player.x = max(0, Player.x - Player.vx );
      }
      if (gb.buttons.repeat(BTN_RIGHT, 1)) {
        Player.x = min(LCDWIDTH - Player.w, Player.x + Player.vx );
      }
      //move the ball
      Ball.x = Ball.x + Ball.vx;
      Ball.y = Ball.y + Ball.vy;
      //collision with the top
      if (Ball.x < 0) {
        Ball.x = 0;
        Ball.vx = -Ball.vx;
        gb.sound.playTick();
      }
      //collision with the left
      if (Ball.y < 0) {
        Ball.y = 0;
        Ball.vy = -Ball.vy;
        gb.sound.playTick();
      }
      //collision with the right
      if ((Ball.x + Ball.size) > LCDWIDTH) {
        Ball.x = LCDWIDTH - Ball.size;
        Ball.vx = -Ball.vx;
        gb.sound.playTick();
      }
      //collision with the bottom
      if ((Ball.y + Ball.size) > LCDHEIGHT) {
        Ball.y = LCDHEIGHT - Ball.size;
        Ball.vy = -Ball.vy;       
        GameReset = true;
        gb.sound.playTick();        
      }
      //collision with the player
      if (gb.collideRectRect(Ball.x, Ball.y, Ball.size, Ball.size, Player.x, Player.y, Player.w, Player.h)) {
        int BallOnPlayer = ((Ball.x + ( Ball.size / 2 )) - Player.x) + 1;
        if (BallOnPlayer > (Player.w / 2))
          Ball.vx = Ball.v;
        else
          Ball.vx = -Ball.v;
        Ball.vy = -Ball.vy;
        gb.sound.playTick();
      }

    }
    
        //draw the ball
    gb.display.fillRect(Ball.x, Ball.y, Ball.size, Ball.size);
    //draw the player
    gb.display.fillRect(Player.x, Player.y, Player.w, Player.h);
    //Draw Bricks
    for (byte row = 0; row < BrickRows; row++)
      for (byte col = 0; col < BrickCols; col++) {
        //draw the bricks if they do exist
        if (Brick[row][col].exist)
          gb.display.fillRect(Brick[row][col].x, Brick[row][col].y, Brick[row][col].w, Brick[row][col].h);
      }
    //collision with the bricks
    for (byte row = 0; row < BrickRows; row++)
      for (byte col = 0; col < BrickCols; col++) {
        //collision with the bricks
        if (gb.collideRectRect(Ball.x, Ball.y, Ball.size, Ball.size, Brick[row][col].x, Brick[row][col].y, Brick[row][col].w, Brick[row][col].h) && Brick[row][col].exist) {
          //Incr Score
          Player.score++;
          //Incr Destroyed Bricks
          DestBrickCount++;
          //Hide the Brick
          Brick[row][col].exist = false;
          Ball.vy = -Ball.vy;
          //Explosion Animation
          byte AnimationDelay=40;
          gb.sound.playCancel();
          gb.display.drawBitmap(Ball.x - Ball.size, Ball.y - Ball.size, explode2);
          gb.display.update();
          delay(AnimationDelay);
          gb.display.drawBitmap(Ball.x - Ball.size, Ball.y - Ball.size, explode1);
          gb.display.update();
          delay(AnimationDelay);
          gb.display.drawBitmap(Ball.x - Ball.size, Ball.y - Ball.size, explode0);
          gb.display.update();
          delay(AnimationDelay);
          gb.display.drawBitmap(Ball.x - Ball.size, Ball.y - Ball.size, explode1);
          gb.display.update();
          delay(AnimationDelay);
          gb.display.drawBitmap(Ball.x - Ball.size, Ball.y - Ball.size, explode2);
          gb.display.update();
          delay(AnimationDelay);
          gb.sound.playTick();
        }
      }
    //No more Bricks exist
    if (DestBrickCount >= BrickCount) {
      GameWin = true;
      GameReset = true;
      DestBrickCount = 0;
    }
  }
}
