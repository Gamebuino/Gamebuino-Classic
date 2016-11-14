#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

//store the different sprites in PROGMEM to save RAM
const byte grass[] PROGMEM = {16, 16, 0x10,0x0,0x28,0x2,0x10,0x0,0x0,0x0,0x0,0x0,0x10,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x80,0x0,0x0,0x40,0x0,0x0,0x0,0x0,0x8,0x4,0x0,0x0,0x0,};
const byte rocks[] PROGMEM = {16, 16, 0x20,0x0,0x50,0x0,0x21,0xe1,0x6,0x18,0x8,0xc,0x8,0x4,0x3c,0x6,0x42,0x7,0x40,0x37,0x40,0x1f,0x23,0x9e,0x1c,0x7e,0x8,0x3c,0x8,0x78,0x7,0xe0,0x0,0x0,};
const byte road_straight[] PROGMEM = {16, 16, 0x1f,0xf8,0x1f,0xf8,0x5e,0x78,0x1e,0x78,0x1e,0x78,0x1e,0x78,0x1e,0x79,0x1e,0x78,0x1f,0xf8,0x1f,0xf8,0x1e,0x78,0x1e,0x78,0x9e,0x78,0x1e,0x78,0x1e,0x78,0x1e,0x78,};
const byte road_turn[] PROGMEM = {16, 16, 0x1f,0xf8,0x1f,0xf8,0x1f,0xfc,0x1f,0xff,0x1f,0xff,0xf,0xff,0xf,0xff,0x7,0xff,0x87,0xff,0x3,0xff,0x1,0xff,0x0,0x7f,0x2,0x1f,0x0,0x0,0x0,0x0,0x40,0x0,};

// array containing the different sprites
#define NUM_SPRITES 4
const byte* sprites[NUM_SPRITES] = {
  grass, rocks, road_straight, road_turn};

/*
The world is stored in bi-dimensional array of bytes.
This array is stored in RAM so it can be too large.
For large maps see the PROGMEM tile map examples.
Each tile of the world is stored in a byte. A byte is made of 8 bits.
The bits B000000XX define the sprite's ID (so there is 4 different sprites max)
The bits B0000XX00 define the sprite rotation (4 values of rotation)
You can notice that 4 bits are unused for each byte (these one: BXXXX0000).
This means that we could save even more memory by storing 2 tiles per byte,
but that would make accessing the the tiles coordinates more complicated.
It will be the subject of another example.
We could also store more than 4 different sprites.
*/
#define WORLD_W 16
#define WORLD_H 8
byte world[WORLD_W][WORLD_H];

byte getSpriteID(byte x, byte y){
  return world[x][y] & B00000011;
}
byte getRotation(byte x, byte y){
  return (world[x][y] >> 2)  & B00000011;
}
void setTile(byte x, byte y, byte spriteID, byte rotation){
  world[x][y] = (rotation << 2) + spriteID;
}

// cursor
int cursor_x, cursor_y;
int camera_x, camera_y;

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
    
    updateCursor();
    
    drawWorld();
    drawCursor();

  }
}

void initGame(){
  gb.titleScreen(F("DYNAMIC TILE MAP DEMO"));
  gb.pickRandomSeed(); //pick a different random seed each time for games to be different
  initWorld();
  gb.popup(F("\25:change \26:rotate"),60);
}

void initWorld(){
  for(byte y = 0; y < WORLD_H; y++){
    for(byte x = 0; x < WORLD_W; x++){
      setTile(x, y, 0, random(0,4)); //fill with grass with random rotation
    }
  }
}

void drawWorld(){
  for(byte y = 0; y < WORLD_H; y++){
    for(byte x = 0; x < WORLD_W; x++){
      byte spriteID = getSpriteID(x,y);
      byte rotation = getRotation(x,y);
      //coordinates on the screen depending on the camera position
      int x_screen = x*16 - camera_x;
      int y_screen = y*16 - camera_y;
      if(x_screen < -16 || x_screen > LCDWIDTH || y_screen < -16 || y_screen > LCDHEIGHT){
        continue; // don't draw sprites which are out of the screen
      }
      gb.display.drawBitmap(x_screen, y_screen, sprites[spriteID], rotation, 0);
    }
  }
}

void updateCursor(){
  byte spriteID = getSpriteID(cursor_x,cursor_y);
  byte rotation = getRotation(cursor_x,cursor_y);
  if(gb.buttons.repeat(BTN_A, 4)){
    spriteID = (spriteID+1) % NUM_SPRITES;
    gb.sound.playOK();
  }
  if(gb.buttons.repeat(BTN_B, 4)){
    rotation = (rotation+1) % 4;
    gb.sound.playOK();
  }
  setTile(cursor_x, cursor_y, spriteID, rotation);
  
  if(gb.buttons.repeat(BTN_RIGHT, 4)){
    cursor_x = wrap(cursor_x+1, WORLD_W);
    gb.sound.playTick();
  }
  if(gb.buttons.repeat(BTN_LEFT, 4)){
    cursor_x = wrap(cursor_x-1, WORLD_W);
    gb.sound.playTick();
  }
  if(gb.buttons.repeat(BTN_DOWN, 4)){
    cursor_y = wrap(cursor_y+1, WORLD_H);
    gb.sound.playTick();
  }
  if(gb.buttons.repeat(BTN_UP, 4)){
    cursor_y = wrap(cursor_y-1, WORLD_H);
    gb.sound.playTick();
  }
  
  //target position of the camera for the cursor to be centered
  int camera_x_target = cursor_x*16 - LCDWIDTH/2 + 8;
  int camera_y_target = cursor_y*16 - LCDHEIGHT/2 + 8;
  //apply the target coordinate to the current coordinates with some smoothing
  camera_x = (camera_x*3 + camera_x_target)/4;
  camera_y = (camera_y*3 + camera_y_target)/4;
  
}

void drawCursor(){
  int x_screen = cursor_x*16 - camera_x;
  int y_screen = cursor_y*16 - camera_y;
  if(!(x_screen < -16 || x_screen > LCDWIDTH || y_screen < -16 || y_screen > LCDHEIGHT)){
    gb.display.drawRect(x_screen, y_screen, 16, 16);
  }
  
  gb.display.print(F("X"));
  gb.display.print(cursor_x);
  gb.display.print(F(" Y"));
  gb.display.print(cursor_y);
  
  byte spriteID = getSpriteID(cursor_x, cursor_y);
  gb.display.print(F(" I"));
  gb.display.print(spriteID);
  byte rotation = getRotation(cursor_x, cursor_y);
  gb.display.print(F(" R"));
  gb.display.print(rotation);
}

