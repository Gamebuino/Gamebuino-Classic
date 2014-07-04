//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//importe the Gamebuino library
#include <Gamebuino.h>
//creates a Gamebuino object named gb
Gamebuino gb;

const byte logo[] PROGMEM =
{
  64, 30,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00011000, B00000000, B00000000, B00000000, B00001000, B00000000, B01110000,
  B00000000, B01111000, B00000000, B00111100, B00011100, B00111000, B00010001, B11110000,
  B00000000, B11111000, B00111000, B01111100, B01111100, B01111000, B00111011, B11110000,
  B00000001, B11111100, B11111000, B01111100, B11111100, B11111000, B01111111, B11110000,
  B00000111, B11111100, B11111100, B11111101, B11111100, B11111100, B01111111, B11000000,
  B00001111, B11111000, B11111100, B11111001, B11111110, B01111101, B11111111, B10000000,
  B00011111, B11100001, B11111110, B11111011, B11111110, B00111101, B11111110, B00000000,
  B00011111, B10000001, B11111110, B11111011, B11011110, B11111111, B11101110, B00000000,
  B00011111, B00000001, B11111110, B11111011, B11011111, B11111111, B11011110, B11100000,
  B00111110, B00011110, B11101111, B01111011, B11001111, B11111111, B10001111, B11100000,
  B00111111, B11111111, B11101111, B11111111, B11111111, B11111111, B11001111, B10000000,
  B00111111, B11111111, B11100111, B11111111, B11111111, B10111111, B11100111, B10111000,
  B00011111, B00011111, B11100011, B11110111, B11111111, B10111111, B11110111, B11110000,
  B00000000, B00111110, B11100001, B11110111, B10000111, B10111110, B11110011, B11110000,
  B00000000, B11111111, B11100001, B11001111, B10000011, B00011110, B01100011, B11000000,
  B00000011, B11111111, B11100000, B00011111, B00000000, B00000000, B01000011, B10000000,
  B00011111, B11111011, B00000000, B00000110, B00000000, B00000000, B00000000, B00000000,
  B00011111, B11111000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00001111, B11100000, B00000000, B00000000, B00000011, B11111100, B11110000, B00000000,
  B00001111, B00000000, B00000000, B00000000, B00011111, B11111111, B11111000, B00000000,
  B00000100, B00000000, B00001111, B11111111, B11111111, B11011111, B11111000, B00000000,
  B00000000, B00000001, B11111111, B11111111, B11111111, B11011111, B01111000, B00000000,
  B00000000, B00000111, B11111111, B11111111, B11111111, B11111111, B11110000, B00000000,
  B00000000, B00011111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000000, B00111111, B11111111, B11111111, B11111111, B11111111, B11111111, B11000000,
  B00000000, B11111111, B11111100, B00000000, B01111111, B11111111, B11111111, B11100000,
  B00000000, B11111111, B00000000, B00000000, B00000111, B11011111, B11111111, B11100000,
  B00000001, B11111110, B00111111, B11110000, B00000011, B11101011, B11111111, B11100000,
  B00000001, B11111111, B11111111, B11111111, B11110001, B11111011, B11111101, B01100000,
};
const byte headL[] PROGMEM = {
  5, 3,
  B00100000,
  B01011000,
  B11110000,
};
const byte headR[] PROGMEM = {
  5, 3,
  B00100000,
  B11010000,
  B01111000,
};
const byte headU[] PROGMEM = {
  3, 5,
  B10000000,
  B11000000,
  B10100000,
  B11000000,
  B01000000,
};
const byte headD[] PROGMEM = {
  3, 5,
  B01000000,
  B11000000,
  B10100000,
  B11000000,
  B10000000,
};

const byte bodyH[] PROGMEM =
{
  3, 3,
  B01000000,
  B11100000,
  B01000000
};
const byte bodyV[] PROGMEM =
{
  3, 3,
  B01000000,
  B11100000,
  B01000000
};

const byte mouse[] PROGMEM =
{
  8, 4,
  B00001100,
  B10011010,
  B10111110,
  B01111111
};
const byte apple[] PROGMEM = {
  8, 8,
  B00001110,
  B00010000,
  B01111110,
  B10011001,
  B10000001,
  B10000001,
  B01100110,
  B00011000,
};

// Game constants
boolean paused = false;
// Direction
#define  TOP    0
#define  RIGHT  1
#define  BOTTOM 2
#define  LEFT   3
//Game Speed
unsigned long prevTime = 0;                        // for gamedelay (ms)
int delayTime = 500;                               // Game step in ms

//Snake
#define MAX_SNAKE_LENGTH 200                       //needs much dynamic memmory!!!!
#define START_SNAKE_LENGTH 2
char direction = TOP;                              // direction of movement
typedef struct
{
  char x;
  char y;
  byte w;
  byte h;
  byte Length;
}  snake;
snake Snake[MAX_SNAKE_LENGTH];

//Food
#define FOOD_TYPES 2
#define MOUSE 0
#define APPLE 1
typedef struct
{
  char x;
  char y;
  byte w;
  byte h;
  byte points;
  byte type;
  boolean exist;
}  food;
food Food[FOOD_TYPES];

void setup() {
  // initialize the Gamebuino object
  gb.begin();
  gb.titleScreen(F("Snake by Ripper121"), logo);
  initGame();
}

void loop(void)
{
  if (gb.update()) {
    if (gb.buttons.pressed(BTN_C)) {
      paused = !paused; //toggle paused state
    }

    if (!paused) {
      //      if (gb.buttons.pressed(BTN_LEFT)) {
      //        direction--;
      //        if (direction < 0) {
      //          direction = LEFT;
      //        }
      //      }
      //
      //      if (gb.buttons.pressed(BTN_RIGHT)) {
      //        direction++;
      //        if (direction > 3) {
      //          direction = TOP;
      //        }
      //      }

      if (gb.buttons.pressed(BTN_LEFT)) {
        direction = LEFT;
      }
      if (gb.buttons.pressed(BTN_RIGHT)) {
        direction = RIGHT;
      }
      if (gb.buttons.pressed(BTN_UP)) {
        direction = TOP;
      }
      if (gb.buttons.pressed(BTN_DOWN)) {
        direction = BOTTOM;
      }

      unsigned long currentTime = millis();
      if (currentTime - prevTime >= delayTime) {
        nextstep();
        prevTime = currentTime;
      }
    }
    draw();
  }
}

void initGame() {
  //Game Speed
  prevTime = 0;                        // for gamedelay (ms)
  delayTime = 200;                     // Game step in ms
  randomSeed(millis());
  //init snake
  makesnake();
  //init Food
  makeFood();
}

void makeFood() {
  for (byte i = 0; i < FOOD_TYPES; i++) {
    Food[i].exist = false;
  }
  char x , y;
  byte randomType = random(0, FOOD_TYPES);
  if (randomType == MOUSE) {
    Food[randomType].w = 8;
    Food[randomType].h = 4;
    Food[randomType].points = 2;
  }
  if (randomType == APPLE) {
    Food[randomType].w = 8;
    Food[randomType].h = 8;
    Food[randomType].points = 1;
  }
  x = random(0, LCDWIDTH - Food[randomType].w);
  y = random(0, LCDHEIGHT - Food[randomType].h);
  while (isPartOfSnake(x, y, Food[randomType].w, Food[randomType].h)) {
    x = random(0, LCDWIDTH - Food[randomType].w);
    y = random(0, LCDHEIGHT - Food[randomType].h);
  }
  Food[randomType].x = x;
  Food[randomType].y = y;
  Food[randomType].type = randomType;
  Food[randomType].exist = true;
}

void makesnake() {
  direction = TOP;                               // direction of movement
  Snake[0].Length = START_SNAKE_LENGTH;          // nr of parts of snake
  Snake[0].w = 4;
  Snake[0].h = 4;
  byte calcH = (LCDHEIGHT / Snake[0].h) / 2;
  byte calcW = (LCDWIDTH / Snake[0].w) / 2;
  Snake[0].x = calcH * Snake[0].h;
  Snake[0].y = calcW * Snake[0].w;
  for (byte i = 1; i < MAX_SNAKE_LENGTH; i++) {
    Snake[i].x = 0 - Snake[0].w;
    Snake[i].y = 0 - Snake[0].w;
    Snake[i].w = Snake[0].w;
    Snake[i].h = Snake[0].h;
    Snake[i].Length = Snake[0].Length;
  }
}

void draw() {
  drawScore();
  drawSnake();
  drawFood();
  gb.display.drawFastVLine(0, 0, LCDHEIGHT);
  gb.display.drawFastHLine(0, 0, LCDWIDTH);
  gb.display.drawFastVLine(LCDWIDTH-1, 0, LCDHEIGHT);
  gb.display.drawFastHLine(0, LCDHEIGHT-1, LCDWIDTH);
}

void drawSnake() {
  for (byte i = 0; i < Snake[0].Length; i++) {
    switch (direction) {
      case TOP:
        if (i == 0)
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, headU);
        else
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, bodyV);
        break;
      case RIGHT:
        if (i == 0)
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, headR);
        else
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, bodyH);
        break;
      case BOTTOM:
        if (i == 0)
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, headD);
        else
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, bodyV);
        break;
      case LEFT:
        if (i == 0)
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, headL);
        else
          gb.display.drawBitmap(Snake[i].x, Snake[i].y, bodyH);
        break;
    }

  }
}

void drawFood() {
  for (byte i = 0; i < FOOD_TYPES; i++) {
    if (Food[i].exist) {
      if (Food[i].type == MOUSE)
        gb.display.drawBitmap(Food[i].x, Food[i].y, mouse);
      if (Food[i].type == APPLE)
        gb.display.drawBitmap(Food[i].x, Food[i].y, apple);
    }
  }
}

void drawScore() {
  gb.display.fontSize = 1;
  gb.display.cursorX = 2;
  gb.display.cursorY = 2;
  gb.display.print(Snake[0].Length - START_SNAKE_LENGTH);
}

void nextstep() {
  for (byte i = Snake[0].Length - 1; i > 0; i--) {
    Snake[i].x = Snake[i - 1].x;
    Snake[i].y = Snake[i - 1].y;
  }

  switch (direction) {
    case TOP:
      Snake[0].y = Snake[0].y - Snake[0].w;
      break;
    case RIGHT:
      Snake[0].x = Snake[0].x + Snake[0].h;
      break;
    case BOTTOM:
      Snake[0].y = Snake[0].y + Snake[0].w;
      break;
    case LEFT:
      Snake[0].x = Snake[0].x - Snake[0].h;
      break;
  }

  SnakeInPlayField();

  for (byte i = 0; i < FOOD_TYPES; i++) {
    if (isPartOfSnake(Food[i].x, Food[i].y, Food[i].w, Food[i].h) && Food[i].exist) {
      Food[i].exist = false;
      Snake[0].Length += Food[i].points;
      if (Snake[0].Length > MAX_SNAKE_LENGTH)
        Snake[0].Length = MAX_SNAKE_LENGTH;
      if (Snake[0].Length < MAX_SNAKE_LENGTH) {
        makeFood();
      }
      else {
        Food[i].x = Food[i].y = -1;
      }
    }
  }

  if (isSnakePartOfSnake()) {
    gb.display.clear();
    gb.display.println("Game Over!");
    gb.display.println();
    gb.display.println("Score:");
    gb.display.println(Snake[0].Length - START_SNAKE_LENGTH);
    gb.display.update();
    initGame();
    delay(3000);
  }

}


boolean isPartOfSnake(char x, char y, byte w, byte h) {
  for (byte i = 0; i < Snake[0].Length - 1; i++) {
    if (gb.collideRectRect(Snake[i].x, Snake[i].y, Snake[i].w, Snake[i].h, x, y, w, h)) {
      return true;
      break;
    }
  }
  return false;
}

boolean isSnakePartOfSnake() {
  for (byte i = 1; i < Snake[0].Length; i++) {
    if (gb.collideRectRect(Snake[0].x, Snake[0].y, Snake[0].w, Snake[0].h, Snake[i].x, Snake[i].y, Snake[i].w, Snake[i].h)) {
      return true;
      break;
    }
  }
  return false;
}

void SnakeInPlayField() {
  if (Snake[0].x < 0) {
    Snake[0].x = LCDWIDTH - Snake[0].w;
  }
  if (Snake[0].y < 0) {
    Snake[0].y = LCDHEIGHT - Snake[0].h;
  }
  if (Snake[0].x > LCDWIDTH - Snake[0].h) {
    Snake[0].x = 0;
  }
  if (Snake[0].y > LCDHEIGHT - Snake[0].w) {
    Snake[0].y = 0;
  }
}
