#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
#include <EEPROM.h>
#include <avr/pgmspace.h>

const uint8_t *_world, *_tiles;
uint8_t  _worldWidth,_worldHeight, _byteWidth;

//PROGMEM uint16_t tetris_main1[] = {0x708E, 0x5C4E, 0x604E, 0x684E, 0x702E, 0x682E, 0x604E, 0x5C4E, 0x547E, 0x5418, 0x544E, 0x604E, 0x708E, 0x684E, 0x604E, 0x5CCE, 0x604E, 0x688E, 0x708E, 0x608E, 0x547E, 0x5414, 0x550E, 0x40, 0x688E, 0x744E, 0x848E, 0x7C4E, 0x744E, 0x70CE, 0x604E, 0x708E, 0x684E, 0x604E, 0x5C7E, 0x10, 0x5C4E, 0x604E, 0x688E, 0x708E, 0x608E, 0x547E, 0x1014, 0x548E, 0x541C, 0x541A, 0x5418, 0x5416, 0x5414, 0x5412, 0x20, 0x410E, 0x310E, 0x390E, 0x2D0E, 0x310E, 0x250E, 0x20FE, 0x2018, 0x2C8E, 0x2C1C, 0x2C1A, 0x2C18, 0x2C16, 0x2C14, 0x2C12, 0x20, 0x410E, 0x310E, 0x390E, 0x2D0E, 0x307E, 0x301E, 0x407E, 0x401E, 0x550E, 0x510E, 0x502C, 0x502A, 0x5028, 0x5026, 0x5024, 0x5022, 0x5040, 0x0000};
//PROGMEM uint16_t tetris_main2[] = {0x5C8E, 0x504E, 0x544E, 0x5C8E, 0x544E, 0x504E, 0x407E, 0x4018, 0x404E, 0x544E, 0x608E, 0x5C4E, 0x544E, 0x504E, 0x404E, 0x504E, 0x544E, 0x5C8E, 0x608E, 0x548E, 0x407E, 0x4014, 0x410E, 0x40, 0x448E, 0x544E, 0x603E, 0x10, 0x601E, 0x10, 0x601E, 0x10, 0x5C4E, 0x544E, 0x30CE, 0x104E, 0x304E, 0x542E, 0x302E, 0x144E, 0x104E, 0x504E, 0x104E, 0x504E, 0x544E, 0x5C4E, 0x504E, 0x604E, 0x504E, 0x544E, 0x104E, 0x107E, 0x1014, 0x108E, 0x101C, 0x101A, 0x1018, 0x1016, 0x1014, 0x1012, 0x20, 0x30FE, 0x3014, 0x250E, 0x2CFE, 0x2C14, 0x210E, 0x24FE, 0x2414, 0x10FE, 0x1014, 0x110E, 0x208E, 0x201C, 0x201A, 0x2018, 0x2016, 0x2014, 0x2012, 0x20, 0x310E, 0x250E, 0x2CFE, 0x2C1E, 0x210E, 0x248E, 0x308E, 0x410E, 0x390E, 0x382C, 0x382A, 0x3828, 0x3826, 0x3824, 0x3822, 0x3840, 0x0000};
//PROGMEM uint16_t tetris_drum[] = {0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x2C4E,0x304E,0x384E,0x40,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0x60,0x702F,0xE0,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x504E,0x104E,0x504E,0x104E,0x504E,0x104E,0x504E,0x104E,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x504E,0x104E,0x504E,0x104E,0x100,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x504E,0x104E,0x504E,0x104E,0x504E,0x104E,0x504E,0x104E,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x544E,0x104E,0x204E,0x104E,0x204E,0x104E,0x100,0x0000};

#define WORLD_W 16
#define WORLD_H 12
static unsigned char PROGMEM world[]=
{
  B11001111,B11001111,
  B10000000,B01000000,
  B00000001,B11111100,
  B00111001,B11100000,
  B00001100,B00000110,
  B10000111,B00011111,
  B11000011,B11000001,
  B10000001,B00000001,
  B00111100,B00110011,
  B00000111,B11100000,
  B10000100,B00001001,
  B11000001,B11001111,
};
static unsigned char PROGMEM tiles[]=
{
  8,8,
  B01010100,
  B00100000,
  B00010100,
  B01000001,
  B10001000,
  B00000100,
  B01010001,
  B00001010,
};
static unsigned char PROGMEM mobSprite[]=
{
  8,8,
  B00000000,
  B00000000,
  B00011000,
  B00111100,
  B00011000,
  B00100100,
  B00000000,
  B00000000,
};
static unsigned char PROGMEM bossSprite[]=
{
  8,8,
  B00000000,
  B00100100,
  B00011000,
  B01111110,
  B00111100,
  B01111110,
  B00111100,
  B00000000,
};
static unsigned char PROGMEM playerSprite[]=
{
  8,8,
  B00000000,
  B00111100,
  B01011010,
  B01111110,
  B01111110,
  B01111110,
  B00111100,
  B00000000,
};
static unsigned char PROGMEM splashSprite[]=
{
  8,8,
  B00000000,
  B00001000,
  B00000000,
  B00011000,
  B00111000,
  B10110100,
  B00000000,
  B00010000,
};
static unsigned char PROGMEM crateSprite[]=
{
  8,8,
  B11111111,
  B10000001,
  B11111111,
  B10100011,
  B11000101,
  B11111111,
  B10000001,
  B11111111,
};
static unsigned char PROGMEM fullHeart[]=
{
  8,8,
  B01101100,
  B11111110,
  B11111110,
  B01111100,
  B00111000,
  B00010000,
  B00000000,
  B00000000,
};
static unsigned char PROGMEM halfHeart[]=
{
  8,8,
  B00000000,
  B00001100,
  B00011100,
  B00011000,
  B00010000,
  B00000000,
  B00000000,
  B00000000,
};
static unsigned char PROGMEM emptyHeart[]=
{
  8,8,
  B01101100,
  B10010010,
  B10000010,
  B01000100,
  B00101000,
  B00010000,
  B00000000,
  B00000000,
};

#define playerW 6
#define playerH 6
byte playerSpeed;
int playerX;
int playerY;
char playerLife;
#define playerLifeMax 6
byte playerDir;
int cameraX;
int cameraY;
byte shake_magnitude;
byte shake_timeLeft;
PROGMEM uint16_t player_damage_sound[] = {
  0x5C5F, 0x0000};

///////////////////////////////////// MOBS
#define NUMMOBS 16
#define INITNUMMOBS 4
#define MOBSRATE 6 //how often is the mob number increased (every X kills)
#define BOSSFREQ 16//one boss every X kills (initially)
#define BOSSRATE 1 //every boss killed, the next one will spawn X kills earlier
byte boss_nextSpawn;
byte boss_freq;
byte activeMobs;
int  mobs_x[NUMMOBS];
int  mobs_y[NUMMOBS];
byte mobs_dir[NUMMOBS];
char mobs_life[NUMMOBS];
byte mobs_size[NUMMOBS];
byte mob_maxLife = 10;
byte boss_maxLife = 100;
#define boss_size 6
#define mob_size 4
//PROGMEM uint16_t mob_damage_sound[] = {0x5C1F, 0x0000};
PROGMEM uint16_t mob_death_sound[] = {
  0x7C1F, 0x0000};


#define NUMSPLASH 16
boolean splash_active[NUMSPLASH];
int splash_x[NUMSPLASH];
int splash_y[NUMSPLASH];
int splash_dir[NUMSPLASH];

///////////////////////////////////// WEAPONS
#define NUMBULLETS 10
int     bullets_x[NUMBULLETS];
int     bullets_y[NUMBULLETS];
byte    bullets_dir[NUMBULLETS];
boolean bullets_active[NUMBULLETS];
byte    bullets_weapon[NUMBULLETS];

int     blast_x;
int     blast_y;
byte    blast_lifespan;
byte    blast_bullet;

#define NUMWEAPONS 5
byte currentWeapon;
byte nextShot;
char* PROGMEM weapon_name[NUMWEAPONS] = {
  ".357","P90", "AK47", "RPG", "MG42"};
byte weapon_size[NUMWEAPONS] = {
  2, 1, 2, 3, 2};
byte weapon_damage[NUMWEAPONS] = {
  10, 2, 3, 5, 4};
byte weapon_rate[NUMWEAPONS] = {
  30, 1, 2, 30, 1};
byte weapon_speed[NUMWEAPONS] = {
  4, 5, 3, 2, 5};
byte weapon_spread[NUMWEAPONS] = {
  1, 2, 1, 0, 2};
byte weapon_ennemyRecoil[NUMWEAPONS] = {
  3, 2, 3, 0, 3};
byte weapon_playerRecoil[NUMWEAPONS] = {
  0, 0, 1, 3, 3};
unsigned int weapon_ammo[NUMWEAPONS] = {
  9999, 500, 300, 20, 150};
unsigned int ammo;
PROGMEM uint16_t magnum_sound[] = {
  0x741F,0x2417,0x2015,0x1C13,0x0000};
PROGMEM uint16_t p90_sound[] = {
  0x4C1F, 0x0000};
PROGMEM uint16_t p90_alternative_sound[] = {
  0x441F, 0x0000};
PROGMEM uint16_t ak47_sound[] = {
  0x241F, 0x0000};
PROGMEM uint16_t mg42_sound[] = {
  0x381F,0x2417,0x2015,0x1C13,0x0000};
PROGMEM uint16_t rpg_sound[] = {
  0x1C13,0x2015,0x2417,0x2BC9,0x0000};
uint16_t* weapons_sounds[NUMWEAPONS] = {
  magnum_sound, p90_sound, ak47_sound, rpg_sound, mg42_sound};
PROGMEM uint16_t blast_sound[] = {
  0x1C1F,0x181F,0x141D,0x101B,0xC19,0x817,0x415,0x13,0x0000};
int crate_x, crate_y;
PROGMEM uint16_t power_up[] = {0x3C1E,0x4C1E,0x581E,0x6C1E,0x7C1E,0x401E,0x501E,0x5C1E,0x701E,0x801E,0x481E,0x581E,0x641E,0x781E,0x881E,0x0000};

///////////////////////////////////// SCORE
#define RANKMAX 8 //store 5 high scores
int score = 0;
//int lastScore = 0;
int kills = 0;
int highscore[RANKMAX];
//byte scoreDisplayTimeLeft;
#define NAMELENGTH 10
char name[RANKMAX][NAMELENGTH+1];

///////////////////////////////////// SETUP
void setup() {
  //randomSeed(analogRead(A2));
  gb.begin(F("Crabator"), bossSprite);
  randomSeed(analogRead(A7)*analogRead(A7)+analogRead(A7));
  //Serial.begin(9600);
  //Serial.println("Gamebuino FTW !");
  loadHighscore();
  displayTitle();
}

///////////////////////////////////// LOOP
void loop() {
  play();
}

///////////////////////////////////// DISPLAY TITLE
void displayTitle(){
  /*while(1){
    if(gb.update()){
      gb.display.setTextSize(2);
      gb.display.setCursor(12,12);
      gb.display.print("ALIEN");
      gb.display.setCursor(6,28);
      gb.display.print("KILLER");
      gb.display.setTextSize(1);
      gb.display.setCursor(4-(gb.getFrameCount()/4)%4,0);
      gb.display.print("<");
      gb.display.setCursor(12,0);
      gb.display.print("START");
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playOK();
        break;
      }
    }
  }*/
}

///////////////////////////////////// SCREEN COORD
boolean screenCoord(int absoluteX, int absoluteY, int &x, int &y){
  x = absoluteX - cameraX + 8;
  x = (x >= 0) ? x%(WORLD_W*8) : WORLD_W*8 + x%(WORLD_W*8);
  x -= 8;
  y = absoluteY - cameraY + 8;
  y = (y >= 0) ? y%(WORLD_H*8) : WORLD_H*8 + y%(WORLD_H*8);
  y -= 8;
  if((x > LCDWIDTH) || (y > LCDHEIGHT))
    return false;
  return true;
}

///////////////////////////////////// WRAP
/*int wrap(int i, int imax){
  i = (i >= 0 ) ? i%(imax) : imax + i%(imax);
  return i;
}*/

#define wrap(i, imax) ((imax+i)%(imax))

///////////////////////////////////// MOVE XYDS
void moveXYDS(int &x, int &y, byte &dir, char speed){
  switch(dir){ //switch case depending on the mob's movement direction
  case 0: //going upward
    y -= speed;
    break;
  case 1: //left
    x -= speed;
    break;
  case 2: //downward
    y += speed;
    break;
  case 3: //right
    x += speed;
    break;
  }
  x = wrap(x, WORLD_W*8);
  y = wrap(y, WORLD_H*8);
}

///////////////////////////////////// DISTANCE BETWEEN
byte distanceBetween(int pos1, int pos2, int worldSize){
  byte dist = abs(pos1 - pos2);
  dist = (dist < worldSize/2) ? dist : worldSize - dist;
  return dist;
}

///////////////////////////////////// ASSIGN ARRAY
void assignArray(char *array1, char *array2, byte length){
  for(byte i=0; i<length; i++)
    array1[i] = array2[i];
}






















