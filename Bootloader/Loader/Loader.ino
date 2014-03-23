#include <SD.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
File root;
char* factorChar[] = {
  "", "K", "M", "G"};
char loadName[13];

void setup(){
  //Serial.begin(9600);
  gb.begin();
  pinMode(10, OUTPUT);

  if (!SD.begin(10)) {
    gb.display.clear();
    gb.display.print("Insert SD card");
    gb.display.println("and restart.");
    gb.display.update();
    while(1);
  }

  root = SD.open("/");
  gb.display.setTextWrap(false);
}

void loop(){
  root.rewindDirectory();
  printDirectory(root);
}

void printDirectory(File &dir) {
  //int currentDirectorySize = getDirectorySize(dir);
  int selectedFile = 0; //number of the selected file 0 for the 1st file, 1 for the 2nd, etc.
  int thisFile = 0; //number of the file currently itering through
  byte factor = 0; //0 for Byte, 1 for KiloByte, etc.
  unsigned long fileSize = 0; //size of the selected file
  while(1)
    if(gb.update()){
      if(gb.buttons.pressed(BTN_B)){
        break;
      }
      if(gb.buttons.repeat(BTN_DOWN,3)){
        selectedFile++;
        /*if(selectedFile >= currentDirectorySize){
         selectedFile = 0;
         }*/
      }
      if(gb.buttons.repeat(BTN_UP,3)){
        selectedFile--;
        if(selectedFile < 0){
          //selectedFile = currentDirectorySize-1;
          selectedFile = 0;
        }
      }
      gb.display.setCursor(0,0);
      gb.display.print(dir.name());
      if(fileSize){
        gb.display.setCursor(54,0);
        if(fileSize<100) //print spaces for right align
          gb.display.print(" ");
        if(fileSize<10)
          gb.display.print(" ");
        if(!factor)
          gb.display.print(" ");

        gb.display.print(fileSize);
        gb.display.print(factorChar[factor]);
        gb.display.print("B");
      }
      gb.display.drawFastHLine(0,8,LCDWIDTH,BLACK);
      gb.display.setCursor(0,10);
      //start from the first file
      dir.rewindDirectory();
      thisFile=0;
      File entry;

      while(1){ //iterate through all the files of the folder

        //don't display files which are out of the screen (below)
        if(thisFile > selectedFile+5)
          break;

        entry = dir.openNextFile();

        //don't display files which are out of the screen (above)
        if(thisFile >= selectedFile-1){
          if (!entry) { //no more files
            if(thisFile == selectedFile){ //the selected file is out of the list
              selectedFile = 0;
            }
            break;
          }

          //display the selector character
          if(thisFile == selectedFile){
            gb.display.print(">");
          }
          else{
            gb.display.print(" ");
          }

          //get selected file size
          if(thisFile == selectedFile){
            if(entry.isDirectory()){
              fileSize = 0;
            }
            else{
              fileSize = entry.size();
              factor = 0;
              while(fileSize>999){
                fileSize /= 1024;
                factor++;
              }
            }
          }

          if(entry.isDirectory())
            gb.display.print("/");

          //open the item selected when A is pressed
          if(thisFile == selectedFile){
            if(gb.buttons.pressed(BTN_A)){
              //open the directory
              if (entry.isDirectory()) {
                printDirectory(entry);
                entry.close();
                break;
              }
              //open the file
              else {
                strcpy(loadName, entry.name());
                gb.display.setCursor(66,40);
                if(isHex(loadName)){
                  gb.display.println("OK!");
                  strcpy(strstr(strupr(loadName), ".HEX"), "");
                  gb.display.clear();
                  gb.display.print("LOADING...");
                  gb.display.update();
                  load_game(loadName);
                }
                else{
                  gb.display.println("NO!");
                }
              }
            }
          }

          gb.display.println(entry.name());
        }
        entry.close();
        thisFile++;
      }
    }
}
/*
int getDirectorySize(File &dir){
 int i=0;
 File entry;
 dir.rewindDirectory();
 while(true){ //go through the directory
 entry =  dir.openNextFile();
 if (!entry) { //no more files
 return i;
 }
 entry.close();
 i++;
 }
 }*/

bool isHex(char* filename) {
  int8_t len = strlen(filename);
  bool result;
  if (strstr(strupr(filename + (len - 4)), ".HEX")
    // and anything else you want
  ) {
    result = true;
  } 
  else {
    result = false;
  }
  return result;
}




