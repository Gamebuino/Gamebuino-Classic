#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
#include <EEPROM.h>
#include <avr/pgmspace.h>

extern const byte font3x5[];
extern const byte font5x7[];

#define WORLD_W 16
#define WORLD_H 12
uint8_t  byteWidth = (WORLD_H + 7) / 8;

//declare all the sprites which are in the "sprites" tab
extern const byte PROGMEM logo[];
extern const byte PROGMEM world[];
extern const byte PROGMEM tiles[];
extern const byte PROGMEM mobSprite[];
extern const byte PROGMEM bossSprite[];
extern const byte PROGMEM playerSprite[];
extern const byte PROGMEM splashSprite[];
extern const byte PROGMEM crateSprite[];
extern const byte PROGMEM fullHeart[];
extern const byte PROGMEM halfHeart[];
extern const byte PROGMEM emptyHeart[];

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
const PROGMEM uint16_t player_damage_sound[] = {0x0045,0x564,0x0000};

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
//const PROGMEM uint16_t mob_damage_sound[] = {0x5C1F, 0x0000};
const PROGMEM uint16_t mob_death_sound[] = {0x0045,0x184,0x0000};

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

const char str357[] PROGMEM = ".357";
const char strP90[] PROGMEM = "P90";
const char strAK47[] PROGMEM = "AK47";
const char strRPG[] PROGMEM = "RPG";
const char strMG42[] PROGMEM = "MG42";
const char*  const weapon_name[NUMWEAPONS] PROGMEM = {
  str357,strP90, strAK47, strRPG, strMG42};
const byte weapon_size[NUMWEAPONS] = {
  2, 1, 2, 3, 2};
const byte weapon_damage[NUMWEAPONS] = {
  10, 2, 3, 5, 4};
const byte weapon_rate[NUMWEAPONS] = {
  30, 1, 2, 30, 1};
const byte weapon_speed[NUMWEAPONS] = {
  4, 5, 3, 2, 5};
const byte weapon_spread[NUMWEAPONS] = {
  1, 2, 1, 0, 2};
const byte weapon_ennemyRecoil[NUMWEAPONS] = {
  3, 2, 3, 0, 3};
const byte weapon_playerRecoil[NUMWEAPONS] = {
  0, 0, 1, 3, 3};
const unsigned int weapon_ammo[NUMWEAPONS] = {
  9999, 500, 300, 20, 150};
unsigned int ammo;

const uint16_t magnum_sound[] PROGMEM = {0x0045,0x7049,0x17C,0x784D,0x42C,0x0000};
const uint16_t p90_sound[] PROGMEM = {0x0045, 0x0154, 0x0000};
const uint16_t p90_alternative_sound[] PROGMEM = {0x0045, 0x014C, 0x0000};
const uint16_t ak47_sound[] PROGMEM = {0x0045, 0x012C, 0x0000};
const uint16_t mg42_sound[] PROGMEM = {0x0045,0x140,0x8141,0x7849,0x788D,0x52C,0x0000};
const uint16_t rpg_sound[] PROGMEM = {0x0045,0x8101,0x7F30,0x0000};

const uint16_t* const weapons_sounds[NUMWEAPONS] PROGMEM= {
  magnum_sound, p90_sound, ak47_sound, rpg_sound, mg42_sound};
const uint16_t blast_sound[] PROGMEM = {0x0045,0x7849,0x784D,0xA28,0x0000};
int crate_x, crate_y;
const uint16_t power_up[] PROGMEM = {0x0005,0x140,0x150,0x15C,0x170,0x180,0x16C,0x154,0x160,0x174,0x184,0x14C,0x15C,0x168,0x17C,0x18C,0x0000};

///////////////////////////////////// SCORE
#define RANKMAX 5 //number of high scores to save
int score = 0;
//int lastScore = 0;
int kills = 0;
int highscore[RANKMAX];
//byte scoreDisplayTimeLeft;
#define NAMELENGTH 10
char name[RANKMAX][NAMELENGTH+1];

///////////////////////////////////// SETUP
void setup() {
  gb.begin();
  gb.display.setFont(font5x7);
  gb.titleScreen(logo);
  gb.pickRandomSeed();
  loadHighscore();
  initGame();
}

///////////////////////////////////// LOOP
void loop() {
  pause();
  gb.titleScreen(logo);
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
