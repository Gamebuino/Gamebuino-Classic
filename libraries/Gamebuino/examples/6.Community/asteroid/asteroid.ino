#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb = Gamebuino();

///////////////////////////////////// GAME VARS
boolean paused = false;
boolean GameOver = false;
float LevelSpeed = 0.0;
int Level = 0;
int Lives = 3;
int AsteroidExist = 0;

typedef struct
{
  float x;
  float y;
  byte w;
  byte h;
  int rotation;
  float v;
  float vx;
  float vy;
  int Score;
}  ShipStruct;
ShipStruct Ship;

#define maxBullets 4
typedef struct
{
  int x;
  int y;
  int rotation;
  byte v;
  int vx;
  int vy;
  boolean exist;
}  bullet;
bullet Bullet[maxBullets];


typedef struct
{
  float x;
  float y;
  byte w;
  byte h;
  float v;
  float vx;
  float vy;
  boolean exist;
}  asteroid;
#define maxBigAsteroid 3
#define maxMediumAsteroid maxBigAsteroid * 3
#define maxSmallAsteroid maxMediumAsteroid * 3
asteroid BigAsteroid[maxBigAsteroid];
asteroid MediumAsteroid[maxMediumAsteroid];
asteroid SmallAsteroid[maxSmallAsteroid];

static unsigned char PROGMEM Logo_black[] =
{
  64, 28,
  B00010000, B00000010, B00000000, B00000000, B00001000, B00010000, B00000000, B00000000,
  B00101000, B00000010, B00000000, B00000000, B00000000, B00010000, B00000000, B00000000,
  B00101000, B01110111, B10011100, B01110011, B10001000, B11110000, B00000000, B00000000,
  B01000100, B10000010, B00100010, B01000100, B01001001, B00010000, B00000000, B00000000,
  B01111100, B01100010, B00111110, B01000100, B01001001, B00010000, B00000000, B00000000,
  B01000100, B00010010, B00100000, B01000100, B01001001, B00010000, B00000000, B00000000,
  B10000010, B11100001, B10011110, B01000011, B10001000, B11110000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000001, B00000000, B00000000, B00000000, B01110000, B00000000,
  B00000001, B11110000, B00000011, B00000000, B00000000, B00000001, B10001000, B00000000,
  B00000010, B00010000, B00000101, B00000000, B00000000, B00000001, B00001000, B00000000,
  B00000100, B00001000, B00001110, B10000000, B00110000, B00000001, B00001000, B00000000,
  B00000100, B00001000, B00000001, B10000000, B01010000, B00000000, B10001000, B00000000,
  B00000100, B00001000, B00000000, B00000000, B01001000, B00000000, B11110000, B00000000,
  B00000100, B00010000, B00000000, B00100000, B00111000, B00000000, B00000000, B00000000,
  B00000010, B00110000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000011, B11100000, B00000000, B00001000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000110, B00000010, B00000000, B00000111, B00000000, B00000000,
  B00000000, B00000000, B00001010, B00000011, B00000000, B00011000, B10000000, B00000000,
  B00000000, B00000000, B00001001, B00000101, B00000000, B00010000, B10000000, B00000000,
  B00000000, B00000000, B00000111, B00000100, B10000000, B00010000, B10000000, B00000000,
  B00000000, B00000000, B00000000, B00000011, B10000000, B00001000, B10000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00001111, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
};

static unsigned char PROGMEM BigAsteroid_black[] =
{
  8, 8,
  B00111100,
  B01000011,
  B10000001,
  B10000001,
  B10000001,
  B10000001,
  B11000110,
  B00111000,
};
static unsigned char PROGMEM MediumAsteroid_black[] =
{
  6, 6,
  B00111000,
  B11000100,
  B10000100,
  B10000100,
  B01000100,
  B01111000,
};
static unsigned char PROGMEM SmallAsteroid_black[] =
{
  4, 4,
  B01100000,
  B10010000,
  B11010000,
  B00110000,
};

static unsigned char PROGMEM NShip_black[] =
{
  5, 5,
  B00100000,
  B00100000,
  B01010000,
  B01010000,
  B11111000,
};
static unsigned char PROGMEM NEShip_black[] =
{
  5, 5,
  B00011000,
  B11101000,
  B01010000,
  B00110000,
  B00010000,
};
static unsigned char PROGMEM EShip_black[] =
{
  5, 5,
  B10000000,
  B11100000,
  B10011000,
  B11100000,
  B10000000,
};
static unsigned char PROGMEM SEShip_black[] =
{
  5, 5,
  B00010000,
  B00110000,
  B01010000,
  B11101000,
  B00011000,
};
static unsigned char PROGMEM SShip_black[] =
{
  5, 5,
  B11111000,
  B01010000,
  B01010000,
  B00100000,
  B00100000,
};
static unsigned char PROGMEM SWShip_black[] =
{
  5, 5,
  B01000000,
  B01100000,
  B01010000,
  B10111000,
  B11000000,
};
static unsigned char PROGMEM WShip_black[] =
{
  5, 5,
  B00001000,
  B00111000,
  B11001000,
  B00111000,
  B00001000,
};
static unsigned char PROGMEM NWShip_black[] =
{
  5, 5,
  B11000000,
  B10111000,
  B01010000,
  B01100000,
  B01000000,
};

///////////////////////////////////// GAME SETUP
void setup() {
  randomSeed(gb.battery.voltage + gb.backlight.ambientLight);
  gb.begin();
  gb.titleScreen(F(".by Ripper121"), Logo_black);
  gb.battery.show = false; //hide the battery indicator
  initGame();
}


///////////////////////////////////// GAME INIT
void initGame() {
  //Init Bullets
  for (byte i = 0; i < maxBullets; i++) {
    Bullet[i].x = LCDWIDTH / 2;
    Bullet[i].y = LCDHEIGHT / 2;
    Bullet[i].v = 2;
    Bullet[i].vx = 0;
    Bullet[i].vy = 0;
    Bullet[i].exist = false;
  }

  //Init Ship
  Ship = (ShipStruct) {
    LCDWIDTH / 2, LCDHEIGHT / 2, 5, 5, 0, 0, 0, 0, 0
  };
  //Init Asteroid
  AsteroidExist = 0;
  //Init BigAsteroid
  for (byte i = 0; i < maxBigAsteroid; i++) {
    if (random(0, 2) == 0) {
      BigAsteroid[i].x = random(0, (LCDWIDTH / 2) - Ship.w * 2);
      BigAsteroid[i].y = random(0, (LCDHEIGHT / 2) - Ship.h * 2);
    } else {
      BigAsteroid[i].x = random((LCDWIDTH / 2) + Ship.w * 2, LCDWIDTH);
      BigAsteroid[i].y = random((LCDHEIGHT / 2) + Ship.h * 2, LCDHEIGHT);
    }
    BigAsteroid[i].w = 8;
    BigAsteroid[i].h = 8;
    BigAsteroid[i].v = 0.1 + LevelSpeed;
    switch (random(0, 8)) {
      case 0:
        BigAsteroid[i].vx = BigAsteroid[i].v;
        BigAsteroid[i].vy = +BigAsteroid[i].v;
        break;
      case 1:
        BigAsteroid[i].vx = -BigAsteroid[i].v;
        BigAsteroid[i].vy = +BigAsteroid[i].v;
        break;
      case 2:
        BigAsteroid[i].vy = 0;
        BigAsteroid[i].vx = -BigAsteroid[i].v;
        break;
      case 3:
        BigAsteroid[i].vx = -BigAsteroid[i].v;
        BigAsteroid[i].vy = -BigAsteroid[i].v;
        break;
      case 4:
        BigAsteroid[i].vy = -BigAsteroid[i].v;
        BigAsteroid[i].vx = 0;
        break;
      case 5:
        BigAsteroid[i].vx = +BigAsteroid[i].v;
        BigAsteroid[i].vy = -BigAsteroid[i].v;
        break;
      case 6:
        BigAsteroid[i].vx = +BigAsteroid[i].v;
        BigAsteroid[i].vy = 0;
        break;
      case 7:
        BigAsteroid[i].vx = +BigAsteroid[i].v;
        BigAsteroid[i].vy = +BigAsteroid[i].v;
        break;
    }
    BigAsteroid[i].exist = true;
  }

  //Init MediumAsteroid
  for (byte i = 0; i < maxMediumAsteroid; i++) {
    MediumAsteroid[i].x = LCDWIDTH / 2;
    MediumAsteroid[i].y = LCDHEIGHT / 2;
    MediumAsteroid[i].w = 6;
    MediumAsteroid[i].h = 6;
    MediumAsteroid[i].v = 0.2 + LevelSpeed;
    switch (random(0, 8)) {
      case 0:
        MediumAsteroid[i].vx = MediumAsteroid[i].v;
        MediumAsteroid[i].vy = +MediumAsteroid[i].v;
        break;
      case 1:
        MediumAsteroid[i].vx = -MediumAsteroid[i].v;
        MediumAsteroid[i].vy = +MediumAsteroid[i].v;
        break;
      case 2:
        MediumAsteroid[i].vy = 0;
        MediumAsteroid[i].vx = -MediumAsteroid[i].v;
        break;
      case 3:
        MediumAsteroid[i].vx = -MediumAsteroid[i].v;
        MediumAsteroid[i].vy = -MediumAsteroid[i].v;
        break;
      case 4:
        MediumAsteroid[i].vy = -MediumAsteroid[i].v;
        MediumAsteroid[i].vx = 0;
        break;
      case 5:
        MediumAsteroid[i].vx = +MediumAsteroid[i].v;
        MediumAsteroid[i].vy = -MediumAsteroid[i].v;
        break;
      case 6:
        MediumAsteroid[i].vx = +MediumAsteroid[i].v;
        MediumAsteroid[i].vy = 0;
        break;
      case 7:
        MediumAsteroid[i].vx = +MediumAsteroid[i].v;
        MediumAsteroid[i].vy = +MediumAsteroid[i].v;
        break;
    }
    MediumAsteroid[i].exist = false;
  }

  //Init SmallAsteroid
  for (byte i = 0; i < maxSmallAsteroid; i++) {
    SmallAsteroid[i].x = LCDWIDTH / 2;
    SmallAsteroid[i].y = LCDHEIGHT / 2;
    SmallAsteroid[i].w = 4;
    SmallAsteroid[i].h = 4;
    SmallAsteroid[i].v = 0.3 + LevelSpeed;
    switch (random(0, 8)) {
      case 0:
        SmallAsteroid[i].vx = SmallAsteroid[i].v;
        SmallAsteroid[i].vy = +SmallAsteroid[i].v;
        break;
      case 1:
        SmallAsteroid[i].vx = -SmallAsteroid[i].v;
        SmallAsteroid[i].vy = +SmallAsteroid[i].v;
        break;
      case 2:
        SmallAsteroid[i].vy = 0;
        SmallAsteroid[i].vx = -SmallAsteroid[i].v;
        break;
      case 3:
        SmallAsteroid[i].vx = -SmallAsteroid[i].v;
        SmallAsteroid[i].vy = -SmallAsteroid[i].v;
        break;
      case 4:
        SmallAsteroid[i].vy = -SmallAsteroid[i].v;
        SmallAsteroid[i].vx = 0;
        break;
      case 5:
        SmallAsteroid[i].vx = +SmallAsteroid[i].v;
        SmallAsteroid[i].vy = -SmallAsteroid[i].v;
        break;
      case 6:
        SmallAsteroid[i].vx = +SmallAsteroid[i].v;
        SmallAsteroid[i].vy = 0;
        break;
      case 7:
        SmallAsteroid[i].vx = +SmallAsteroid[i].v;
        SmallAsteroid[i].vy = +SmallAsteroid[i].v;
        break;
    }
    SmallAsteroid[i].exist = false;
  }
}



///////////////////////////////////// GAME LOOP
void loop() {

  //Game Run
  if (gb.update()) {

    //pause the game if C is pressed
    if (gb.buttons.pressed(BTN_C)) {
      paused = !paused; //toggle paused state
    }
    if (!paused) {
      if (!GameOver) {
        gb.display.cursorX = 0;
        gb.display.cursorY = 0;
        gb.display.print(Ship.Score);
        gb.display.cursorX = 8;
        gb.display.cursorY = 0;
        gb.display.print("/");
        gb.display.cursorX = 12;
        gb.display.cursorY = 0;
        gb.display.print(maxBigAsteroid + maxMediumAsteroid + maxSmallAsteroid);
        gb.display.cursorX = 81;
        gb.display.cursorY = 0;
        gb.display.print(Lives);

        //rotate the Ship
        if (gb.buttons.pressed(BTN_LEFT)) {
          Ship.rotation--;
        }
        if (gb.buttons.pressed(BTN_RIGHT)) {
          Ship.rotation++;
        }
        if (Ship.rotation > 7)
          Ship.rotation = 0;
        if (Ship.rotation < 0)
          Ship.rotation = 7;

        //move the Ship
        if (gb.buttons.pressed(BTN_UP)) {
          Ship.v -= 0.1;
        }
        if (gb.buttons.pressed(BTN_DOWN)) {
          Ship.v += 0.1;
        }
        if (Ship.v > 1.8)
          Ship.v = 1.8;
        if (Ship.v < -1.8)
          Ship.v = -1.8;

        Ship.x += Ship.vx;
        Ship.y += Ship.vy;

        //Ship collision with the top
        if (Ship.x < 0) {
          Ship.x = LCDWIDTH;
        }
        //Ship collision with the left
        if (Ship.y < 0) {
          Ship.y = LCDHEIGHT;
        }
        //Ship collision with the right
        if (Ship.x > LCDWIDTH) {
          Ship.x = 0;
        }
        //Ship collision with the bottom
        if (Ship.y > LCDHEIGHT) {
          Ship.y = 0;
        }




        //Bullets Create
        if (gb.buttons.pressed(BTN_A)) {
          gb.sound.playOK();
          for (byte i = 0; i < maxBullets; i++) {
            if (!Bullet[i].exist)
            {
              Bullet[i].x = Ship.x + Ship.w / 2;
              Bullet[i].y = Ship.y + Ship.h / 2;
              Bullet[i].rotation = Ship.rotation;
              Bullet[i].exist = true;
              break;
            }
          }
        }

        //Bullet Move, Destroy, Draw
        for (byte i = 0; i < maxBullets; i++) {
          //Destroy Bullet
          if (Bullet[i].x < 0 || Bullet[i].y < 0 || Bullet[i].x > LCDWIDTH || Bullet[i].y > LCDHEIGHT) {
            Bullet[i].x = LCDWIDTH / 2;
            Bullet[i].y = LCDHEIGHT / 2;
            Bullet[i].vx = 0;
            Bullet[i].vy = 0;
            Bullet[i].exist = false;
            gb.sound.playTick();
          }
          //Move Bullet
          if (Bullet[i].exist)
          {
            Bullet[i].y += Bullet[i].vx;
            Bullet[i].x += Bullet[i].vy;
            drawBullet(Bullet[i].rotation, i);
          }
        }

        //SmallAsteroid
        AsteroidExist = 0;
        for (byte i = 0; i < maxSmallAsteroid; i++) {
          if (SmallAsteroid[i].exist) {
            AsteroidExist++;
            SmallAsteroid[i].x += SmallAsteroid[i].vx;
            SmallAsteroid[i].y += SmallAsteroid[i].vy;
            if (SmallAsteroid[i].x < 0) {
              SmallAsteroid[i].x = LCDWIDTH;
            }
            //collision with the left
            if (SmallAsteroid[i].y < 0) {
              SmallAsteroid[i].y = LCDHEIGHT;
            }
            //collision with the right
            if (SmallAsteroid[i].x > LCDWIDTH) {
              SmallAsteroid[i].x = 0;
            }
            //collision with the bottom
            if (SmallAsteroid[i].y > LCDHEIGHT) {
              SmallAsteroid[i].y = 0;
            }
            //Ship collision with the SmallAsteroid
            if (gb.collideRectRect(Ship.x, Ship.y, Ship.w, Ship.h, SmallAsteroid[i].x, SmallAsteroid[i].y, SmallAsteroid[i].w, SmallAsteroid[i].h)) {
              Ship.Score++;
              Lives--;
              if (Lives <= 0)
                GameOver = true;
              SmallAsteroid[i].exist = false;
              gb.sound.playCancel();
              break;
            }
            //Bullet collision with the SmallAsteroid
            for (byte j = 0; j < maxBullets; j++) {
              if (gb.collideRectRect(Bullet[j].x, Bullet[j].y, 1, 1, SmallAsteroid[i].x, SmallAsteroid[i].y, SmallAsteroid[i].w, SmallAsteroid[i].h) && Bullet[j].exist) {
                Ship.Score++;
                Bullet[j].exist = false;
                SmallAsteroid[i].exist = false;
                gb.sound.playTick();
              }
            }
            gb.display.drawBitmap(SmallAsteroid[i].x, SmallAsteroid[i].y, SmallAsteroid_black);
          }
        }

        //MediumAsteroid
        for (byte i = 0; i < maxMediumAsteroid; i++) {
          if (MediumAsteroid[i].exist) {
            AsteroidExist++;
            MediumAsteroid[i].x += MediumAsteroid[i].vx;
            MediumAsteroid[i].y += MediumAsteroid[i].vy;
            if (MediumAsteroid[i].x < 0) {
              MediumAsteroid[i].x = LCDWIDTH;
            }
            //collision with the left
            if (MediumAsteroid[i].y < 0) {
              MediumAsteroid[i].y = LCDHEIGHT;
            }
            //collision with the right
            if (MediumAsteroid[i].x > LCDWIDTH) {
              MediumAsteroid[i].x = 0;
            }
            //collision with the bottom
            if (MediumAsteroid[i].y > LCDHEIGHT) {
              MediumAsteroid[i].y = 0;
            }
            //Ship collision with the MediumAsteroid
            if (gb.collideRectRect(Ship.x, Ship.y, Ship.w, Ship.h, MediumAsteroid[i].x, MediumAsteroid[i].y, MediumAsteroid[i].w, MediumAsteroid[i].h)) {
              Ship.Score++;
              Lives--;
              if (Lives <= 0)
                GameOver = true;
              MediumAsteroid[i].exist = false;
              for (byte k = 0; k < 3; k++) {
                for (byte l = 0; l < maxSmallAsteroid; l++) {
                  if (!SmallAsteroid[l].exist) {
                    SmallAsteroid[l].x = MediumAsteroid[i].x;
                    SmallAsteroid[l].y = MediumAsteroid[i].y;
                    SmallAsteroid[l].exist = true;
                    break;
                  }
                }
              }
              gb.sound.playCancel();
              break;
            }
            //Bullet collision with the MediumAsteroid
            for (byte j = 0; j < maxBullets; j++) {
              if (gb.collideRectRect(Bullet[j].x, Bullet[j].y, 1, 1, MediumAsteroid[i].x, MediumAsteroid[i].y, MediumAsteroid[i].w, MediumAsteroid[i].h) && Bullet[j].exist) {
                Ship.Score++;
                Bullet[j].exist = false;
                MediumAsteroid[i].exist = false;
                for (byte k = 0; k < 3; k++) {
                  for (byte l = 0; l < maxSmallAsteroid; l++) {
                    if (!SmallAsteroid[l].exist) {
                      SmallAsteroid[l].x = MediumAsteroid[i].x;
                      SmallAsteroid[l].y = MediumAsteroid[i].y;
                      SmallAsteroid[l].exist = true;
                      break;
                    }
                  }
                }
                gb.sound.playTick();
              }
            }
            gb.display.drawBitmap(MediumAsteroid[i].x, MediumAsteroid[i].y, MediumAsteroid_black);
          }
        }

        //BigAsteroid
        for (byte i = 0; i < maxBigAsteroid; i++) {
          if (BigAsteroid[i].exist) {
            AsteroidExist++;
            BigAsteroid[i].x += BigAsteroid[i].vx;
            BigAsteroid[i].y += BigAsteroid[i].vy;
            if (BigAsteroid[i].x < 0) {
              BigAsteroid[i].x = LCDWIDTH;
            }
            //collision with the left
            if (BigAsteroid[i].y < 0) {
              BigAsteroid[i].y = LCDHEIGHT;
            }
            //collision with the right
            if (BigAsteroid[i].x > LCDWIDTH) {
              BigAsteroid[i].x = 0;
            }
            //collision with the bottom
            if (BigAsteroid[i].y > LCDHEIGHT) {
              BigAsteroid[i].y = 0;
            }
            //Ship collision with the BigAsteroid
            if (gb.collideRectRect(Ship.x, Ship.y, Ship.w, Ship.h, BigAsteroid[i].x, BigAsteroid[i].y, BigAsteroid[i].w, BigAsteroid[i].h)) {
              Ship.Score++;
              Lives--;
              if (Lives <= 0)
                GameOver = true;
              BigAsteroid[i].exist = false;
              for (byte k = 0; k < 3; k++) {
                for (byte l = 0; l < maxMediumAsteroid; l++) {
                  if (!MediumAsteroid[l].exist) {
                    MediumAsteroid[l].exist = true;
                    MediumAsteroid[l].x = BigAsteroid[i].x;
                    MediumAsteroid[l].y = BigAsteroid[i].y;
                    break;
                  }
                }
              }
              gb.sound.playCancel();
              break;
            }
            //Bullet collision with the BigAsteroid
            for (byte j = 0; j < maxBullets; j++) {
              if (gb.collideRectRect(Bullet[j].x, Bullet[j].y, 1, 1, BigAsteroid[i].x, BigAsteroid[i].y, BigAsteroid[i].w, BigAsteroid[i].h) && Bullet[j].exist) {
                Ship.Score++;
                Bullet[j].exist = false;
                BigAsteroid[i].exist = false;
                for (byte k = 0; k < 3; k++) {
                  for (byte l = 0; l < maxMediumAsteroid; l++) {
                    if (!MediumAsteroid[l].exist) {
                      MediumAsteroid[l].exist = true;
                      MediumAsteroid[l].x = BigAsteroid[i].x;
                      MediumAsteroid[l].y = BigAsteroid[i].y;
                      break;
                    }
                  }
                }
                gb.sound.playTick();
              }
            }
            gb.display.drawBitmap(BigAsteroid[i].x, BigAsteroid[i].y, BigAsteroid_black);
          }
        }


        //Check if all Asteroids are Destroyed
        if (AsteroidExist <= 0) {
          GameOver = false;
          LevelSpeed += 0.1;
          Ship.Score = 0;
          Level++;
          gb.display.clear();
          gb.display.println(F("Next Level:"));
          gb.display.println(Level);
          gb.display.println(F("!!!Higher Speed!!!"));
          gb.display.update();
          delay(1000);
          initGame();
        }

        drawShip(Ship.rotation);
      }
      else
      {
        gb.display.clear();
        gb.display.println(F("Game OVER"));
        gb.display.update();
        delay(1000);
        GameOver = false;
        LevelSpeed = 0.0;
        Level = 0;
        Lives = 3;
        Ship.Score = 0;
        initGame();
      }
    }
  }
}



///////////////////////////////////// GAME Functions
void drawShip(int rotation) {
  switch (rotation)
  {
    case 0:
      Ship.vx = 0;
      Ship.vy = +Ship.v;
      gb.display.drawBitmap(Ship.x, Ship.y, NShip_black);
      break;
    case 1:
      Ship.vx = -Ship.v;
      Ship.vy = +Ship.v;
      gb.display.drawBitmap(Ship.x, Ship.y, NEShip_black);
      break;
    case 2:
      Ship.vy = 0;
      Ship.vx = -Ship.v;
      gb.display.drawBitmap(Ship.x, Ship.y, EShip_black);
      break;
    case 3:
      Ship.vx = -Ship.v;
      Ship.vy = -Ship.v;
      gb.display.drawBitmap(Ship.x, Ship.y, SEShip_black);
      break;
    case 4:
      Ship.vy = -Ship.v;
      Ship.vx = 0;
      gb.display.drawBitmap(Ship.x, Ship.y, SShip_black);
      break;
    case 5:
      Ship.vx = +Ship.v;
      Ship.vy = -Ship.v;
      gb.display.drawBitmap(Ship.x, Ship.y, SWShip_black);
      break;
    case 6:
      Ship.vx = +Ship.v;
      Ship.vy = 0;
      gb.display.drawBitmap(Ship.x, Ship.y, WShip_black);
      break;
    case 7:
      Ship.vx = +Ship.v;
      Ship.vy = +Ship.v;
      gb.display.drawBitmap(Ship.x, Ship.y, NWShip_black);
      break;
  }
  return;
}

void drawBullet(byte rotation, byte i) {
  switch (rotation)
  {
    case 0:
      Bullet[i].vx = -Bullet[i].v;
      Bullet[i].vy = 0;
      break;
    case 1:
      Bullet[i].vx = -Bullet[i].v;
      Bullet[i].vy = +Bullet[i].v;
      break;
    case 2:
      Bullet[i].vy = +Bullet[i].v;
      Bullet[i].vx = 0;
      break;
    case 3:
      Bullet[i].vx = +Bullet[i].v;
      Bullet[i].vy = +Bullet[i].v;
      break;
    case 4:
      Bullet[i].vy = 0;
      Bullet[i].vx = +Bullet[i].v;
      break;
    case 5:
      Bullet[i].vx = +Bullet[i].v;
      Bullet[i].vy = -Bullet[i].v;
      break;
    case 6:
      Bullet[i].vx = 0;
      Bullet[i].vy = -Bullet[i].v;
      break;
    case 7:
      Bullet[i].vx = -Bullet[i].v;
      Bullet[i].vy = -Bullet[i].v;

      break;
  }
  gb.display.drawPixel(Bullet[i].x, Bullet[i].y);
  return;
}
