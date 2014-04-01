#include <tinyFAT.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

char fileName[9];
byte initres;
byte res;
int numberOfFiles = 1;
int numberOfPages;
int selectedFile;
int selectedPage;
int prevSelectedPage;
int thisFile;
#define PAGELENGTH 6

char fileNameEeprom[13];
#define BUFFER_SIZE 128
char buffer_eeprom[BUFFER_SIZE+4];

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
            gb.display.println(F("EEPROM \x1A SD"));
            gb.display.update();
            saveeeprom();
            gb.display.println(F("SD \x1A EEPROM"));
            gb.display.update();
            //loadeeprom();
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
  gb.display.fillRect(0,0,6,LCDHEIGHT, WHITE);
  gb.display.setCursor(0,8*(selectedFile%PAGELENGTH));
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
    gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT,BLACK);
    gb.display.fillRect(LCDWIDTH-3, selectedPage*LCDHEIGHT/numberOfPages, 3, 1+LCDHEIGHT/numberOfPages, BLACK);
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
  strcpy(fileNameEeprom, fileName);
  for(byte i=0; i<8; i++){
    if(fileNameEeprom[i] == ' ')
      fileNameEeprom[i] = '\0';
  }
  strcat(fileNameEeprom, ".MEM");
  if(file.exists(fileNameEeprom))
    file.delFile(fileNameEeprom);
  file.create(fileNameEeprom);
  res=file.openFile(fileNameEeprom, FILEMODE_TEXT_WRITE);
  if (res==NO_ERROR)
  {
    for(byte i=0; i< 1024/BUFFER_SIZE; i++){
      buffer_eeprom[BUFFER_SIZE+1] = '\0';
      for(byte j = 0; j<BUFFER_SIZE; j+=2){
        buffer_eeprom[j] = 0x0F | EEPROM.read((i*BUFFER_SIZE+j)/2);
        buffer_eeprom[j+1] = 0xF0 |  EEPROM.read((i*BUFFER_SIZE+j)/2);
      }
      file.writeLn(buffer_eeprom);
    }
    file.closeFile();
  }
  else{
    gb.display.println(F("Error"));
    gb.display.update();
  }
}

void loadeeprom(){
  res=file.openFile("BACKUP.MEM", FILEMODE_TEXT_READ);
  if (res==NO_ERROR)
  {
    word result=0;
    byte i = 0;
    while ((result!=EOF) and (result!=FILE_IS_EMPTY))
    {
      result=file.readLn(buffer_eeprom, BUFFER_SIZE+2);
      if (result!=FILE_IS_EMPTY)
      {
        for(byte j=0; j<BUFFER_SIZE; j+=2){
          EEPROM.write((i*BUFFER_SIZE+j)/2,(buffer_eeprom[j] & 0xF0) | (buffer_eeprom[j+1] & 0x0F));
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
