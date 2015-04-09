#include <tinyFAT.h> //requires the tinyFAT library. You can download it here : http://www.henningkarlsen.com/electronics/library.php?id=37
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

extern const byte logo[] PROGMEM;

char nextGameName[9] = "\0\0\0\0\0\0\0\0";
byte initres;
byte res;
int numberOfFiles;
int numberOfPages;
int selectedFile;
int selectedPage = 0;
int prevSelectedPage = 0;
#define PAGEWIDTH 5
#define PAGEHEIGHT 3
#define PAGELENGTH (PAGEWIDTH*PAGEHEIGHT)
char thisPageFiles[PAGELENGTH][9];
uint16_t thisPageClusters[PAGELENGTH];
byte cursorPos = 0;
byte oldCursorPos;
byte filesOnPage;

char completeName[13] = "xxxxxxxx.xxx";
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE+4];
byte flashCounter = 0;

void setup(){
  //Serial.begin(115200);
  gb.begin();
  gb.startMenuTimer = 12;
  gb.titleScreen(logo);
  gb.display.clear();
  gb.display.persistence=true;
  gb.battery.show = false;
  
  gb.display.print(F("\35 Reading SD card...\n\n"));
  gb.display.update();

  SPI.setClockDivider(SPI_CLOCK_DIV128); //lower the SPI speed for better compatibility
  initres=file.initFAT();

  if (initres!=NO_ERROR)
  {
    gb.display.clear();
    gb.display.print(F("Insert SD card\nand restart."));
    gb.display.update();
    while(1);
  }
  
  // using nextGameName to save 9 bytes of RAM
  const char* address = SETTINGS_PAGE + OFFSET_CURRENTGAME;
  nextGameName[0] = pgm_read_byte(address);
  nextGameName[1] = pgm_read_byte(address+1);
  nextGameName[2] = pgm_read_byte(address+2);
  nextGameName[3] = pgm_read_byte(address+3);
  nextGameName[4] = pgm_read_byte(address+4);
  nextGameName[5] = pgm_read_byte(address+5);
  nextGameName[6] = pgm_read_byte(address+6);
  nextGameName[7] = pgm_read_byte(address+7);

  for(byte i=0; i<8; i++){
    if(nextGameName[i] == ' ')
      nextGameName[i] = '\0';
  }

  if(nextGameName[0]){
    saveeeprom();
    saveName();
  }
  /*else{
   gb.display.println(F("No prev game found"));
   }*/

  /*gb.display.println(F("\25:continue"));
   gb.display.update();
   while(1){
   gb.buttons.update();
   if(gb.buttons.pressed(BTN_A)) break;
   delay(50);
   }*/

  //count the number of files
  file.findFirstFile(&file.DE);
  while(res == NO_ERROR){
    res = file.findNextFile(&file.DE);
    if(res != NO_ERROR) break;
    if(doDispFile()){
      numberOfFiles++;
    }
  }
  numberOfPages = ((numberOfFiles-1)/PAGELENGTH) + 1;
  gb.display.textWrap = false;
  updateList();
}

void loop(){
  while(1)
    if(gb.update()){
      // maybe check for left/right boundries?
      if(gb.buttons.pressed(BTN_A)){
        loadSelectedFile();
      }
      if(gb.buttons.repeat(BTN_RIGHT,3)){
        cursorPos++;
        if(cursorPos >= filesOnPage){
          cursorPos = 0;
          selectedPage++;
          if(selectedPage >= numberOfPages){
            selectedPage = 0;
          }
        }else{
          updateCursor();
        }
      }
      if(gb.buttons.repeat(BTN_DOWN,3)){
        cursorPos += PAGEWIDTH;
        if(cursorPos >= filesOnPage || gb.buttons.repeat(BTN_B,1)){
          cursorPos %= PAGEWIDTH;
          
          selectedPage++;
          if(selectedPage >= numberOfPages){
            selectedPage = 0;
          }
        }else{
          updateCursor();
        }
      }

      if(gb.buttons.repeat(BTN_LEFT,3)){
        if(cursorPos == 0 ){ // so that we don't have to compare with negative numbers
          cursorPos = PAGELENGTH-1; // updating the list will adjust this if on last page
          if(selectedPage == 0){
            selectedPage = numberOfPages; // we will get decreased one after this if-condition anyways
          }
          selectedPage--;
        }else{
          cursorPos--;
          updateCursor();
        }
      }
      if(gb.buttons.repeat(BTN_UP,3)){
        if(cursorPos < PAGEWIDTH || gb.buttons.repeat(BTN_B,1)){ // so that we don't have to compare with negative numbers
          cursorPos += (PAGEWIDTH * (PAGEHEIGHT - 1)); // updating the list will adjust this if on last page
          if(selectedPage == 0){
            selectedPage = numberOfPages; // we will get decreased one after this if-condition anyways
          }
          selectedPage--;
        }else{
          cursorPos -= PAGEWIDTH;
          updateCursor();
        }
      }

      if(selectedPage != prevSelectedPage){
        prevSelectedPage = selectedPage;
        updateList();
      }
      if(flashCounter++ == 10){ // do the flashing
        flashCounter = 0;
        gb.display.setColor(INVERT); // just invert of whatever was there before
        drawCursorBox(cursorPos);
      }
    }
}

void saveName(){
  //gb.display.println(F("Saving name to flash"));
  //gb.display.update();
  for(byte i=0; i<128; i++){
    buffer[i] = pgm_read_byte(SETTINGS_PAGE+i);
  }
  for(byte i=0; i<9; i++){
    buffer[i+OFFSET_CURRENTGAME] = nextGameName[i];
  }
  write_flash_page (SETTINGS_PAGE, (unsigned char *)buffer);
}

void loadSelectedFile(){
  // no need to check if the HEX file exists, as we created the thisPageFiles array with searching for hex files
  strcpy(nextGameName,thisPageFiles[cursorPos]);
  gb.display.clear();
  saveName();
  loadeeprom();
  
  gb.display.print(F("\n\35 Flashing game...\n\nDON'T TURN OFF!"));
  gb.display.update();
  load_game(nextGameName);
}






