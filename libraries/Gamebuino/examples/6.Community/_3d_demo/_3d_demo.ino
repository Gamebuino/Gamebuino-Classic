// Gamebuino 3D rendering demo - by Mark Feldman (aka "Myndale" on the Gamebuino forums)
//
// PCD8544 initialization code based on examples at http://playground.arduino.cc/Code/PCD8544
//
// This code has been written to be relatively easy to follow but there's been virtually no attempt to optimize it whatsoever.
// If I were to optimize it I would start by rotating the back buffer 90 degrees and switching the LCD to the alternate
// mode where you write a full column at a time. Next I would hard-code functions to draw a line of wall pixels for each size
// that the wall can be drawn at, plus a jump table to call the appropriate handler. This would eliminate the need for a
// full-screen back-buffer altogether.
//
// The visible-surface detemination code needs an overhaul, it just draws rectangles of blocks from the camera cell out without
// much attempt to cull those outside the view frustum or behind other cells that have already been rendered.
//
// Finally the sprite draw routine could also do with a rewrite, the current version uses fixed-point arithmetic which is much
// faster to do in assembly than in C.

#include <TimerOne.h>    // get this from http://playground.arduino.cc/Code/Timer1

//load games from the SD card using the bootloader
#define load_game (*((void(*)(const char* filename))(0x7ffc/2)))

// how often to update the screen, 41 seems to be the magic number to minimize flicker
#define FRAME_RATE 41

#define PIN_SCE   A1
#define PIN_RESET A0
#define PIN_DC    A2
#define PIN_SDIN  11
#define PIN_SCLK  13
#define PIN_BACKLIGHT 5

#define BTN_UP_PIN      9
#define BTN_RIGHT_PIN   7
#define BTN_DOWN_PIN    6
#define BTN_LEFT_PIN    8
#define BTN_A_PIN       4
#define BTN_B_PIN       2
#define BTN_C_PIN       A3

#define PORT_SCLK PORTB
#define SCLK_BIT 5
#define PORT_SDIN PORTB
#define SDIN_BIT 3

#define LCD_C     LOW
#define LCD_D     HIGH

#define LCD_X     84
#define LCD_Y     48
#define LCD_CMD   0

#define FOV  30
#define NEAR_PLANE (0.5/tan(PI*FOV/180))
#define CLIP_PLANE 0.01f
#define CAMERA_SCALE 50
#define WALL_HEIGHT 0.8f
#define OBJECT_SIZE 0.8f

volatile boolean frameReady = false;
byte buffer[LCD_X*LCD_Y/8];
float wbuffer[LCD_X];
int numColumns;

// this is a very inefficient way of storing the map, it
// would be much better to store it with 1 cell per bit.
// that way you could easily store it in RAM and have
// bigger levels and more than 1.
#define MAP_SIZE 16
const char theMap[MAP_SIZE*MAP_SIZE+1] PROGMEM =
{
  "################"
  "#.#............#"
  "#.#..#.........#"
  "#.#..#.........#"
  "#....#....#....#"
  "##.#.#.........#"
  "######.........#"
  "#..............#"
  "##########..####"
  "#........#.....#"
  "#.....#..#..#..#"
  "#.....#.....#..#"
  "#.....##########"
  "#..............#"
  "#.#.#.#.#......#"
  "################"
};

#define SWAP(A, B) B, A,

// raw data for the monster sprite
const byte sprite[] =
{
  SWAP(B00000000, B00000000)
  SWAP(B11111111, B11111000)
  SWAP(B01000000, B00001100)
  SWAP(B00101000, B00000110)
  SWAP(B01001100, B00000010)
  SWAP(B10000110, B01110011)
  SWAP(B01000010, B10001001)
  SWAP(B00100010, B10101001)
  SWAP(B01000010, B10001001)
  SWAP(B10000110, B01110011)
  SWAP(B01001100, B00000010)
  SWAP(B00101000, B00000110)
  SWAP(B01000000, B00001100)
  SWAP(B11111111, B11111000)
  SWAP(B00000000, B00000000)
  SWAP(B00000000, B00000000)
};

// the mask for the monster sprite indicating which pixels should be drawn
const byte mask[] =
{
  SWAP(B00000000, B00000000)
  SWAP(B11111111, B11111000)
  SWAP(B01111111, B11111100)
  SWAP(B00111111, B11111110)
  SWAP(B01111111, B11111110)
  SWAP(B11111111, B11111111)
  SWAP(B01111111, B11111111)
  SWAP(B00111111, B11111111)
  SWAP(B01111111, B11111111)
  SWAP(B11111111, B11111111)
  SWAP(B01111111, B11111110)
  SWAP(B00111111, B11111110)
  SWAP(B01111111, B11111100)
  SWAP(B11111111, B11111000)
  SWAP(B00000000, B00000000)
  SWAP(B00000000, B00000000)
};


void LcdClear(void) {
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++)
  {
    LcdWrite(LCD_D, 0x00);
  }
}

void LcdInitialise(void) {
  pinMode(PIN_SCE,   OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC,    OUTPUT);
  pinMode(PIN_SDIN,  OUTPUT);
  pinMode(PIN_SCLK,  OUTPUT);

  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);

  LcdWrite( LCD_CMD, 0x21 );  // LCD Extended Commands.
  LcdWrite( LCD_CMD, 0xB9 );  // Set LCD Vop (Contrast). //B1
  LcdWrite( LCD_CMD, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite( LCD_CMD, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite( LCD_CMD, 0x0C );  // LCD in normal mode. 0x0d for inverse
  LcdWrite(LCD_C, 0x20);
  LcdWrite(LCD_C, 0x0C);
}

void LcdWrite(byte dc, byte data) {
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

void gotoXY(int x, int y) {
  LcdWrite( 0, 0x80 | x);  // Column.
  LcdWrite( 0, 0x40 | y);  // Row.  
}

void setup(void) {
  LcdInitialise();
  LcdClear();
  pinMode(PIN_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_BACKLIGHT, HIGH);
  
  pinMode(BTN_UP_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BTN_DOWN_PIN, INPUT_PULLUP);
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_A_PIN, INPUT_PULLUP);
  pinMode(BTN_B_PIN, INPUT_PULLUP);
  pinMode(BTN_C_PIN, INPUT_PULLUP);
   
  // draw the first frame  
  drawFrame();
  
  Timer1.initialize(1000000/FRAME_RATE);         // initialize timer1, and set a 1/2 second period
  Timer1.pwm(9, 512);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(callback);  // attaches callback() as a timer overflow interrupt
}

#define bitOut(val, bit) {                       \
  if (val & (1 << bit))                          \
    PORT_SDIN |= _BV(SDIN_BIT);                  \
  else                                           \
    PORT_SDIN &= ~_BV(SDIN_BIT);                 \
  PORT_SCLK |= _BV(SCLK_BIT);                    \
  PORT_SCLK &= ~_BV(SCLK_BIT);                   \
}

inline void byteOut(byte val)
{
  bitOut(val, 0);
  bitOut(val, 1);
  bitOut(val, 2);
  bitOut(val, 3);
  bitOut(val, 4);
  bitOut(val, 5);
  bitOut(val, 6);
  bitOut(val, 7);
}

void callback() {
  if (frameReady)
  {
    digitalWrite(PIN_DC, LCD_D);
    digitalWrite(PIN_SCE, LOW);
    for (int i=0; i<504; i++)
      byteOut(buffer[i]);
    digitalWrite(PIN_SCE, HIGH);
    frameReady = false;
  }
}

float xpos = 1.5f;
float zpos = 1.5f;
int xcell = 1;
int zcell = 1;
float dir = 0.0f;
float cos_dir;
float sin_dir;

#define MOVEMENT 0.1f
#define TURN 0.1f

void loop(void) {
  boolean redraw = false;
  boolean strafe = (digitalRead(BTN_A_PIN)==LOW);
  float movement = MOVEMENT;
  float turn = TURN;
  
  if (digitalRead(BTN_C_PIN)==LOW)
  {
    load_game("LOADER");
  }
  
  if (digitalRead(BTN_B_PIN)==LOW)
  {
    movement *= 2.0f;
    turn *= 2.0f;
  }
  
  if (digitalRead(BTN_DOWN_PIN)==LOW)
  {
    xpos -= movement * sin_dir;
    zpos -= movement * cos_dir;
    redraw = true;
  }
  
  if (digitalRead(BTN_UP_PIN)==LOW)
  {
    xpos += movement * sin_dir;
    zpos += movement * cos_dir;
    redraw = true;
  }
  
  if (digitalRead(BTN_LEFT_PIN)==LOW)
  {
    if (strafe)
    {
      xpos -= movement * cos_dir;
      zpos += movement * sin_dir;
    }
    else
      dir -= turn;
    redraw = true;
  }
  
  if (digitalRead(BTN_RIGHT_PIN)==LOW)
  {
    if (strafe)
    {
      xpos += movement * cos_dir;
      zpos -= movement * sin_dir;
    }
    else
      dir += turn;
    redraw = true;
  }
  
  xpos = max(xpos, 0);
  zpos = max(zpos, 0);
  xpos = min(xpos, MAP_SIZE);
  zpos = min(zpos, MAP_SIZE);
  
  if (redraw)
    drawFrame();    
}

boolean isValid(int cellX, int cellZ)
{
  if (cellX < 0)
    return false;
  if (cellX >= MAP_SIZE)
    return false;
  if (cellZ < 0)
    return false;
  if (cellZ >= MAP_SIZE)
    return false;
  return true;
}

boolean isBlocked(int cellX, int cellZ)
{
  return pgm_read_byte(theMap + cellZ * MAP_SIZE + cellX) == '#';
}

void drawFrame()
{
  while (frameReady);      // wait for any previous frame to stop drawing
  cos_dir = cos(dir);
  sin_dir = sin(dir);
  xcell = floor(xpos);
  zcell = floor(zpos);
  initWBuffer();
  drawFloorAndCeiling();  
  for (int layer=1; (layer<MAP_SIZE) && (numColumns<LCD_X); layer++)
    drawLayer(layer);
  drawMonster(3.5f, 1.5f);
  frameReady = true;       // copy this frame to the LCD  
}

void initWBuffer()
{
  for (int i=0; i<LCD_X; i++)
    wbuffer[i] = 0.0f;
  numColumns = 0;
}

void drawFloorAndCeiling()
{
  memset(buffer, 0xff, 3*84);
  for (int y=3, ofs=3*84; y<6; y++)
    for (int x=0; x<84; x+=2)
    {
      buffer[ofs++] = 0x55;
      buffer[ofs++] = 0xaa;
    }
}

void drawLayer(int layer)
{
  drawCell(xcell, zcell-layer);
  drawCell(xcell, zcell+layer);
  drawCell(xcell-layer, zcell);
  drawCell(xcell+layer, zcell);
  for (int i=1; i<layer; i++)
  {
    drawCell(xcell-i, zcell+layer);
    drawCell(xcell+i, zcell-layer);
    drawCell(xcell-layer, zcell-i);
    drawCell(xcell+layer, zcell+i);
  }
  for (int i=1; (i<layer) && (numColumns<LCD_X); i++)
  {
    drawCell(xcell+i, zcell+layer);
    drawCell(xcell-i, zcell-layer);
    drawCell(xcell-layer, zcell+i);
    drawCell(xcell+layer, zcell-i);
  }
  drawCell(xcell-layer, zcell+layer);
  drawCell(xcell+layer, zcell+layer);
  drawCell(xcell-layer, zcell-layer);
  drawCell(xcell+layer, zcell-layer);
}

void drawCell(int cellX, int cellZ)
{
  if (!isValid(cellX, cellZ))
    return;
  if (!isBlocked(cellX, cellZ))
    return;
  if (zpos < cellZ)
  {
    if (xpos > cellX)
    {
      // north west quadrant
      if ((zpos < cellZ) && !isBlocked(cellX, cellZ-1))
        drawWall(cellX, cellZ, cellX+1, cellZ);  // south wall
      if ((xpos > cellX+1) && (!isBlocked(cellX+1, cellZ)))
        drawWall(cellX+1, cellZ, cellX+1, cellZ+1);  // east wall
    }
    else
    {
      // north east quadrant
      if ((zpos < cellZ) && !isBlocked(cellX, cellZ-1))
        drawWall(cellX, cellZ, cellX+1, cellZ);  // south wall
      if ((xpos< cellX) && !isBlocked(cellX-1, cellZ))
        drawWall(cellX, cellZ+1, cellX, cellZ);  // west wall
    }
  }
  else
  {
    if (xpos > cellX)
    {
      // south west quadrant
      if ((zpos > cellZ+1) && !isBlocked(cellX, cellZ+1))
        drawWall(cellX+1, cellZ+1, cellX, cellZ+1);  // north wall
      if ((xpos > cellX+1) && !isBlocked(cellX+1, cellZ))
        drawWall(cellX+1, cellZ, cellX+1, cellZ+1);  // east wall
    }
    else
    {
      // south east quadrant
      if ((zpos > cellZ+1) && !isBlocked(cellX, cellZ+1))
        drawWall(cellX+1, cellZ+1, cellX, cellZ+1);  // north wall
      if ((xpos< cellX) && !isBlocked(cellX-1, cellZ))
        drawWall(cellX, cellZ+1, cellX, cellZ);  // west wall
    }
  }
}

inline void setPixel(int x, int y)
{
  buffer[(y >> 3) * 84 + x] |= (0x80 >> (y & 7));
}

inline void clearPixel(int x, int y)
{
  buffer[(y >> 3) * 84 + x] &= ~(0x80 >> (y & 7));
}

// draws one side of a cell
void drawWall(float _x1, float _z1, float _x2, float _z2)
{
  // find position of wall edges relative to eye
  float x1 = cos_dir * (_x1-xpos) - sin_dir * (_z1-zpos);
  float z1 = sin_dir * (_x1-xpos) + cos_dir * (_z1-zpos);
  float x2 = cos_dir * (_x2-xpos) - sin_dir * (_z2-zpos);
  float z2 = sin_dir * (_x2-xpos) + cos_dir * (_z2-zpos);
  
  // clip to the front pane
  if ((z1<CLIP_PLANE) && (z2<CLIP_PLANE))
    return;
  if (z1 < CLIP_PLANE)
  {
    x1 += (CLIP_PLANE-z1) * (x2-x1) / (z2-z1);
    z1 = CLIP_PLANE;
  }
  else if (z2 < CLIP_PLANE)
  {
    x2 += (CLIP_PLANE-z2) * (x1-x2) / (z1-z2);
    z2 = CLIP_PLANE;
  }
  
  // apply perspective projection
  float vx1 = x1 * NEAR_PLANE * CAMERA_SCALE / z1;  
  float vx2 = x2 * NEAR_PLANE * CAMERA_SCALE / z2; 
  
  // transform the end points into screen space
  int sx1 = (int)ceil((LCD_X / 2) + vx1);
  int sx2 = (int)ceil((LCD_X / 2) + vx2) - 1;
  
  // clamp to the visible portion of the screen
  int firstx = max(sx1, 0);
  int lastx = min(sx2, LCD_X-1);
  if (lastx < firstx)
    return;
  
  // loop across each column
  float w1 = 1.0f / z1;
  float w2 = 1.0f / z2;
  float delta_w = (w2 - w1) / (sx2-sx1);
  float w = w1 + (firstx - sx1) * delta_w;
  for (int x=firstx; x<=lastx; x++, w+=delta_w)
  {
    if (w > wbuffer[x])
    {        
      wbuffer[x] = w;
      numColumns++;
      
      // calculate top and bottom
      float vy = (WALL_HEIGHT / 2.0) * NEAR_PLANE * CAMERA_SCALE * w;
      int sy1 = (int)ceil(LCD_Y / 2 - vy);
      int sy2 = (int)ceil(LCD_Y / 2 + vy) - 1;
  
      // clamp to the visible portion of the screen
      int firsty = max(sy1, 0);
      int lasty = min(sy2, LCD_Y-1);
      
      // draw this column
      if ((x==sx1) || (x==sx2))
        for (int y=firsty; y<=lasty; y++)
          setPixel(x, y);
        else
          for (int y=firsty; y<=lasty; y++)
            clearPixel(x, y);
      if (sy1 >= 0)
        setPixel(x, sy1);
      if (sy2 < LCD_Y)
        setPixel(x, sy2);
    }
  }
}

// I have done a little bit of optimization here, namely the use of 4:12 fixed-point arithmetic.
void drawMonster(float _x, float _z)
{
  // find position relative to eye
  float x = cos_dir * (_x-xpos) - sin_dir * (_z-zpos);
  float z = sin_dir * (_x-xpos) + cos_dir * (_z-zpos);
  
  // make sure it's in front of us
  if (z < CLIP_PLANE)
    return;
  float w = 1.0f / z;

  // project the mid-point onto the screen
  float midx = x * NEAR_PLANE * CAMERA_SCALE * w;
  float screen_size = 0.5f * OBJECT_SIZE * CAMERA_SCALE * w;
  int sx1 = (int)ceil(LCD_X / 2 + midx - screen_size);
  int sx2 = (int)ceil(LCD_X / 2 + midx + screen_size) - 1;
  int sy1 = (int)ceil(LCD_Y / 2 - screen_size);
  int sy2 = (int)ceil(LCD_Y / 2 + screen_size) - 1;
  
  int firstx = max(sx1, 0);
  int lastx = min(sx2, LCD_X-1);
  if (firstx > lastx)
    return;
  int firsty = max(sy1, 0);
  int lasty = min(sy2, LCD_Y-1);
  
  unsigned deltax = 0x10000 / (sx2-sx1+1);
  unsigned deltay = 0x10000 / (sy2-sy1+1);
  unsigned first_srcy = 0x10000 * (firsty-sy1) / (sy2-sy1+1);
  unsigned srcx = 0x10000 * (firstx-sx1) / (sx2-sx1+1);
  for (int sx=firstx; sx<=lastx; sx++, srcx+=deltax)
    if (w >= wbuffer[sx])
    {      
      unsigned line = ((unsigned *)&sprite)[srcx>>12];
      unsigned line_mask = ((unsigned *)&mask)[srcx>>12];
      unsigned srcy = first_srcy;
      for (int sy=firsty; sy<=lasty; sy++, srcy+=deltay)
      {
        unsigned bit_mask = 1<<(srcy>>12);
        if (line_mask & bit_mask)
        {
          if (line & bit_mask)
            setPixel(sx, sy);
          else
            clearPixel(sx, sy);
        }
      }
    }
}
