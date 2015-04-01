#include <tinyFAT.h> //requires the tinyFAT library. You can download it here : http://www.henningkarlsen.com/electronics/library.php?id=37
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

extern const byte logo[] PROGMEM;

char nextGameName[9] = "\0\0\0\0\0\0\0\0";
char prevGameName[9] = "zzzzzzzz";
byte initres;
byte res;
int numberOfFiles = 1;
int numberOfPages;
int selectedFile;
int selectedPage;
int prevSelectedPage;
int thisFile;
#define PAGELENGTH (LCDHEIGHT/gb.display.fontHeight)

char completeName[13] = "xxxxxxxx.xxx";
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE+4];

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

  const char* address = SETTINGS_PAGE + OFFSET_CURRENTGAME;
  prevGameName[0] = pgm_read_byte(address);
  prevGameName[1] = pgm_read_byte(address+1);
  prevGameName[2] = pgm_read_byte(address+2);
  prevGameName[3] = pgm_read_byte(address+3);
  prevGameName[4] = pgm_read_byte(address+4);
  prevGameName[5] = pgm_read_byte(address+5);
  prevGameName[6] = pgm_read_byte(address+6);
  prevGameName[7] = pgm_read_byte(address+7);

  for(byte i=0; i<8; i++){
    if(prevGameName[i] == ' ')
      prevGameName[i] = '\0';
  }

  if(prevGameName[0]){
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
    numberOfFiles++;
  }
  numberOfPages = ((numberOfFiles-1)/PAGELENGTH) + 1;
  gb.display.textWrap = false;
  updateList();
}

void loop(){
  selectedFile = 0; //number of the selected file 0 for the 1st file, 1 for the 2nd, etc.
  thisFile = 0; //number of the file currently itering through
  while(1)
    if(gb.update()){

      if(gb.buttons.pressed(BTN_A)){
        loadSelectedFile();
      }

      if(gb.buttons.repeat(BTN_DOWN,3)){
        selectedFile++;
        if(gb.buttons.repeat(BTN_B,1)){
          selectedFile += PAGELENGTH-1;
        }
        if(selectedFile >= numberOfFiles){
          selectedFile = 0;
        }
        selectedPage = selectedFile/PAGELENGTH;
        updateCursor();
      }

      if(gb.buttons.repeat(BTN_UP,3)){
        selectedFile--;
        if(gb.buttons.repeat(BTN_B,1)){
          selectedFile -= PAGELENGTH-1;
        }
        if(selectedFile < 0){
          selectedFile = numberOfFiles-1;
          thisFile = 0;
        }
        selectedPage = selectedFile/PAGELENGTH;
        updateCursor();
      }

      if(selectedPage != prevSelectedPage){
        prevSelectedPage = selectedPage;
        updateList();
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
  byte thisFile = 0;
  res = file.findFirstFile(&file.DE);
  while(res == NO_ERROR){
    if(selectedFile == thisFile){
      //check that this is an HEX file
      if(strstr(file.DE.fileext,"HEX")){
        strcpy(nextGameName, file.DE.filename);
        file.closeFile();
        gb.display.clear();
        saveName();
        loadeeprom();
        /*gb.display.println(F("\25:continue"));
         gb.display.update();
         while(1){
         gb.buttons.update();
         if(gb.buttons.pressed(BTN_A)) break;
         delay(50);
         }*/
        gb.display.print(F("\n\35 Flashing game...\n\nDON'T TURN OFF!"));
        gb.display.update();
        load_game(nextGameName);
      }
      else{ //not an HEX file
        file.closeFile();
        gb.sound.playCancel();
        //draw frame
        gb.display.setColor(WHITE);
        gb.display.fillRoundRect(5,10,LCDWIDTH-10,gb.display.fontHeight*3, 3);
        gb.display.setColor(BLACK);
        gb.display.drawRoundRect(5,10,LCDWIDTH-10,gb.display.fontHeight*3, 3);
        //draw error message
        gb.display.cursorX = 0;
        gb.display.cursorY = 10+3;
        gb.display.println("   Not an HEX file ");
        gb.display.println("   \25: OK ");
        //wait for A to be pressed
        while(1){
          if(gb.update()){
            if(gb.buttons.pressed(BTN_A)){
              gb.display.setColor(BLACK);
              updateList();
              return;
            }
          }
        }
      }
    }
    thisFile++;
    res = file.findNextFile(&file.DE);
  }
}






