#include <tinyFAT.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

#define write_flash_page (*((void(*)(prog_char * page, unsigned char * buffer))(0x7ffa/2)))
// use the second-to-last page in Flash, in Gamebuino the last one contains user settings
#define TARGET_PAGE ((prog_char *)(0x7000-128))

char fileName[9];
char nextGameName[9];
char prevGameName[9];
byte initres;
byte res;
int numberOfFiles = 1;
int numberOfPages;
int selectedFile;
int selectedPage;
int prevSelectedPage;
int thisFile;
#define PAGELENGTH 6

char completeName[13];
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE+4];

void setup(){
  Serial.begin(115200);
  gb.begin(F("Game loader"));
  gb.battery.display(false);

  initres=file.initFAT();
  if (initres!=NO_ERROR)
  {
    gb.display.clear();
    gb.display.print(F("Insert SD card"));
    gb.display.println(F("and restart."));
    gb.display.update();
    while(1);
  }

  /*gb.display.clear();
  gb.display.println(F("Saving prev game"));
  gb.display.update();
  saveeeprom();*/

  file.findFirstFile(&file.DE);
  while(file.findNextFile(&file.DE) == NO_ERROR){
    numberOfFiles++;
  }
  numberOfPages = 1+numberOfFiles/PAGELENGTH;
  gb.display.setTextWrap(false);
  gb.display.persistence=true;
  updateList();
}

void loop(){  //int currentDirectorySize = getDirectorySize(dir);
  selectedFile = 0; //number of the selected file 0 for the 1st file, 1 for the 2nd, etc.
  thisFile = 0; //number of the file currently itering through
  while(1)
    if(gb.update()){

      if(gb.buttons.pressed(BTN_A)){
        byte thisFile = 0;
        res = file.findFirstFile(&file.DE);
        while(res == NO_ERROR){
          if(selectedFile == thisFile){
            strcpy(fileName, file.DE.filename);
            file.closeFile();
            gb.display.clear();
            /*gb.display.println(F("SD \x1A EEPROM"));
            gb.display.update();
            loadeeprom();*/
            gb.display.print(F("Loading game"));
            gb.display.update();
            load_game(fileName);
          }
          thisFile++;
          res = file.findNextFile(&file.DE);
        }
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

void updateCursor(){
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,FONTWIDTH,LCDHEIGHT);
  gb.display.setColor(BLACK, WHITE);
  gb.display.setCursor(0,FONTHEIGHT*(selectedFile%PAGELENGTH));
  gb.display.print("\x10");
  /*gb.display.setCursor(0,40);
   gb.display.print(selectedFile+1);
   gb.display.print("/");
   gb.display.print(numberOfFiles);
   gb.display.print(" ");
   gb.display.print(selectedPage+1);
   gb.display.print("/");
   gb.display.print(numberOfPages);
   gb.display.print(" ");*/
}

void updateList(){
  gb.display.clear();
  if((selectedFile < thisFile) || (thisFile == 0)){ //when going to the previous page
    thisFile = 0;
    res = file.findFirstFile(&file.DE);
    //iterate through previous pages
    for(byte thisPage = 0; thisPage < selectedFile/PAGELENGTH; thisPage++){
      //iterate through files of previous pages
      for(byte i = 0; i<PAGELENGTH; i++){
        file.findNextFile(&file.DE);
        thisFile++;
      }
    }
  }

  if(numberOfFiles > PAGELENGTH){ //if there is several pages
    gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT);
    gb.display.fillRect(LCDWIDTH-3, selectedPage*LCDHEIGHT/numberOfPages, 3, 1+LCDHEIGHT/numberOfPages);
  }

  while (1)
  {
    gb.display.print(" ");
    gb.display.print(file.DE.filename);
    gb.display.print(".");
    gb.display.println(file.DE.fileext);
    thisFile++;
    //open next file
    res = file.findNextFile(&file.DE);
    if(res != NO_ERROR){
      //selectedFile = thisFile-1;
      break;
    }
    //don't display next page
    if(thisFile > ((selectedFile/PAGELENGTH)*PAGELENGTH+PAGELENGTH-1))
      break;
  }
  updateCursor();
}

void saveeeprom(){
  strcpy(completeName, prevGameName);
  for(byte i=0; i<8; i++){
    if(completeName[i] == ' ')
      completeName[i] = '\0';
  }
  strcat(completeName, ".SAV");
  if(file.exists(completeName)){
    file.delFile(completeName);
    gb.display.println("Existing overwritten");
  }
  file.create(completeName);
  res=file.openFile(completeName, FILEMODE_TEXT_WRITE);
  if (res==NO_ERROR)
  {
    for(byte i=0; i< 1024/BUFFER_SIZE; i++){
      buffer[BUFFER_SIZE+1] = '\0';
      for(byte j = 0; j<BUFFER_SIZE; j+=2){
        buffer[j] = 0x0F | EEPROM.read((i*BUFFER_SIZE+j)/2);
        buffer[j+1] = 0xF0 |  EEPROM.read((i*BUFFER_SIZE+j)/2);
      }
      file.writeLn(buffer);
    }
    file.closeFile();
    gb.display.print(F("Saved to "));
    gb.display.println(completeName);
    gb.display.update();
  }
  else{
    gb.display.println(F("Error"));
    gb.display.update();
  }
  delay(500);
}

void loadeeprom(){
  res=file.openFile("BACKUP.MEM", FILEMODE_TEXT_READ);
  if (res==NO_ERROR)
  {
    word result=0;
    byte i = 0;
    while ((result!=EOF) and (result!=FILE_IS_EMPTY))
    {
      result=file.readLn(buffer, BUFFER_SIZE+2);
      if (result!=FILE_IS_EMPTY)
      {
        for(byte j=0; j<BUFFER_SIZE; j+=2){
          EEPROM.write((i*BUFFER_SIZE+j)/2,(buffer[j] & 0xF0) | (buffer[j+1] & 0x0F));
        }
        i++;
      }
      else
        gb.display.println(F("File empty"));
      Serial.println();
    }
    file.closeFile();
  }
  else{
    gb.display.println(F("Error"));
    gb.display.update();
  }
}

