const int chipSelect = 10;
#include <SdFat.h> // requires sdfat, get it at https://github.com/greiman/SdFat

#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;
SdFat sd;
SdFile file;

extern const byte logo[] PROGMEM;

const byte logo[] PROGMEM = {64,36,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0xF8,0xFF,0xFF,0xFC,0x0,0x0,0x0,0x3,0xF9,0xFF,0xFF,0xFE,0x0,0x0,0x0,0x7,0xFF,0xFF,0xFF,0xFF,0x0,0x0,0x7,0xFF,0xFF,0xFF,0xFF,0xFF,0x0,0x0,0xF,0xFF,0x3,0x30,0xC7,0x1F,0x0,0x0,0x1C,0x6,0x1,0x20,0x82,0xF,0x0,0x0,0x1C,0x6,0x49,0xE7,0x92,0x4F,0x0,0x0,0x1F,0xFE,0x49,0x27,0x9E,0x4F,0x0,0x0,0x1C,0x6,0x49,0x27,0x9E,0x4F,0x0,0x0,0x1C,0x6,0x79,0x20,0x9E,0xF,0x0,0x0,0x1F,0xFE,0x79,0x30,0x9F,0x1F,0x0,0x0,0x1C,0x6,0x79,0xFF,0xFF,0xFF,0x0,0x0,0x1C,0x7,0xFF,0x0,0x41,0xBF,0x0,0x0,0x1F,0xFF,0xFC,0x0,0x46,0x6F,0x0,0x0,0x18,0x7,0xF8,0x7F,0xFD,0x87,0x0,0x0,0x18,0x7,0xFC,0xF,0xFE,0x3,0x0,0x0,0x1F,0xFF,0xFE,0x3,0xFE,0x3,0x0,0x0,0x1C,0x7,0xFF,0xC1,0x80,0x7,0x0,0x0,0x1C,0x7,0xF0,0x1,0x80,0x1F,0x0,0x0,0x1F,0xFF,0xE0,0x3,0x0,0x7F,0x0,0x0,0x18,0x7,0xE0,0xF,0x7,0xFF,0x0,0x0,0x18,0x7,0xFF,0xFF,0xFF,0xFF,0x0,0x0,0x1F,0xFF,0xFF,0xFF,0xFF,0xFF,0x0,0x0,0x1C,0x7,0xFF,0x7F,0xFB,0xFF,0x0,0x0,0x1C,0x7,0xFF,0x77,0x33,0x73,0x0,0x0,0x1F,0xFF,0xFF,0x6A,0xAA,0xAF,0x0,0x0,0x1C,0x7,0xFF,0x6A,0xAA,0x6F,0x0,0x0,0x1C,0x7,0xFF,0xB7,0x33,0x2F,0x0,0x0,0xF,0xFF,0xFF,0xFF,0xFF,0xFE,0x0,0x0,0x7,0xFF,0xFF,0xFF,0xFF,0xFC,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,};
const byte floppy8x8[] PROGMEM = {8, 8, 0xCA, 0xCB, 0xC3, 0xFF, 0x81, 0x81, 0x81, 0xFF,};

char nextGameName[9] = "\0\0\0\0\0\0\0\0";
byte initres;
byte res;
int numberOfFiles;
int numberOfPages;
int selectedFile;
int selectedPage = 0;
int prevSelectedPage = 0;
#define PAGE_W 4
#define PAGE_H 2
#define PAGELENGTH (PAGE_W*PAGE_H)

#define TOKENSIZE 5
char token[TOKENSIZE + 1] = "gbinf";

#define NAMELENGTH 18

#define ICON_W 19
#define ICON_BYTEW ((ICON_W + 7) / 8)
#define ICON_H 18
#define ICON_LENGTH (ICON_BYTEW * ICON_H)

#define SLIDE_W 84
#define SLIDE_BYTEW ((SLIDE_W + 7) / 8)
#define SLIDE_H 32
#define SLIDE_LENGTH (SLIDE_BYTEW * SLIDE_H)

#define VERSIONOFFSET (TOKENSIZE + 1)
#define NAMEOFFSET (VERSIONOFFSET + 1)
#define ICONOFFSET (NAMEOFFSET + NAMELENGTH + 1)
#define SLIDENUMBEROFFSET ICONOFFSET + ICON_LENGTH
#define SLIDEOFFSET (SLIDENUMBEROFFSET + 1)

#define HEADERSIZE ((TOKENSIZE+1) + 1 + (NAMELENGTH+1) + ICON_LENGTH + 1)

char thisPageFiles[PAGELENGTH][9];
uint16_t thisPageClusters[PAGELENGTH];

byte cursorPos = 0;
byte oldCursorPos;
byte filesOnPage;

char completeName[13] = "xxxxxxxx.xxx";
char halfName[5] = "XXXX";
char fileExt[4] = "\0\0\0";
#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE + 4];

void setup() {
  //Serial.begin(115200);
  gb.begin();
  gb.battery.thresholds[0] = 0; //disable battery monitoring
  gb.startMenuTimer = 12;
  gb.titleScreen(logo);
  gb.display.clear();
  gb.display.persistence = true;
  gb.battery.show = false;

  gb.display.drawBitmap(0, 12, logo);
  printTopHeader(F("\35 Reading SD card...\n\n"));
  gb.display.update();

  //SPI.setClockDivider(SPI_CLOCK_DIV128); //lower the SPI speed for better compatibility

  if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
    printTopHeader(F("Insert SD & restart"));
    gb.display.update();
    while (1);
  }

  // using nextGameName to save 9 bytes of RAM
  const char* address = SETTINGS_PAGE + OFFSET_CURRENTGAME;
  nextGameName[0] = pgm_read_byte(address);
  nextGameName[1] = pgm_read_byte(address + 1);
  nextGameName[2] = pgm_read_byte(address + 2);
  nextGameName[3] = pgm_read_byte(address + 3);
  nextGameName[4] = pgm_read_byte(address + 4);
  nextGameName[5] = pgm_read_byte(address + 5);
  nextGameName[6] = pgm_read_byte(address + 6);
  nextGameName[7] = pgm_read_byte(address + 7);

  for (byte i = 0; i < 8; i++) {
    if (nextGameName[i] == ' ')
      nextGameName[i] = '\0';
  }

  if (nextGameName[0]) {
    saveeeprom();
    saveName();
  }

  //count the number of files
  sd.chdir('/');
  while (file.openNext(sd.vwd(), O_READ)) {
    if (doDispFile()) {
      numberOfFiles++;
    }
    file.close();
  }

  numberOfPages = ((numberOfFiles - 1) / PAGELENGTH) + 1;
  gb.display.textWrap = false;
  updateList();
}

void loop() {
  while (1)
    if (gb.update()) {
      // maybe check for left/right boundries?
      if (gb.buttons.pressed(BTN_A)) {
        gb.sound.playTick();
        displayGameScreen();
        updateList();
      }
      if (gb.buttons.repeat(BTN_RIGHT, 3)) {
        cursorPos++;
        if (cursorPos >= filesOnPage) {
          cursorPos = 0;
          selectedPage++;
          if (selectedPage >= numberOfPages) {
            selectedPage = 0;
          }
        } else {
          updateCursor();
        }
      }
      if (gb.buttons.repeat(BTN_DOWN, 3)) {
        cursorPos += PAGE_W;
        if (cursorPos >= filesOnPage || gb.buttons.repeat(BTN_B, 1)) {
          cursorPos %= PAGE_W;

          selectedPage++;
          if (selectedPage >= numberOfPages) {
            selectedPage = 0;
          }
        } else {
          updateCursor();
        }
      }

      if (gb.buttons.repeat(BTN_LEFT, 3)) {
        if (cursorPos == 0 ) { // so that we don't have to compare with negative numbers
          cursorPos = PAGELENGTH - 1; // updating the list will adjust this if on last page
          if (selectedPage == 0) {
            selectedPage = numberOfPages; // we will get decreased one after this if-condition anyways
          }
          selectedPage--;
        } else {
          cursorPos--;
          updateCursor();
        }
      }
      if (gb.buttons.repeat(BTN_UP, 3)) {
        if (cursorPos < PAGE_W || gb.buttons.repeat(BTN_B, 1)) { // so that we don't have to compare with negative numbers
          cursorPos += (PAGE_W * (PAGE_H - 1)); // updating the list will adjust this if on last page
          if (selectedPage == 0) {
            selectedPage = numberOfPages; // we will get decreased one after this if-condition anyways
          }
          selectedPage--;
        } else {
          cursorPos -= PAGE_W;
          updateCursor();
        }
      }

      if (selectedPage != prevSelectedPage) {
        updateList();
        prevSelectedPage = selectedPage;
      }
      // draw the blinking selection box
      gb.display.setColor(BLACK);
      if ((gb.frameCount % 8) >= 4) {
        gb.display.setColor(WHITE);
      }
      drawCursorBox(cursorPos);
    }
}

void saveName() {
  //saves the game's name to the flash memory the write_flash_page function from the bootloader
  for (byte i = 0; i < 128; i++) {
    buffer[i] = pgm_read_byte(SETTINGS_PAGE + i);
  }
  for (byte i = 0; i < 9; i++) {
    buffer[i + OFFSET_CURRENTGAME] = nextGameName[i];
  }
  write_flash_page (SETTINGS_PAGE, (unsigned char *)buffer);
}

void loadSelectedFile() {
  // no need to check if the HEX file exists, as we created the thisPageFiles array with searching for hex files
  strcpy(nextGameName, thisPageFiles[cursorPos]);
  saveName();
  loadeeprom();

  //show the first slide from the game's inf file while loading
  drawSlide(0);

  printTopHeader(F("  \35 Loading game...  "));
  printBottomHeader(F(" ! DON'T TURN OFF ! "));
  gb.display.update();
  load_game(nextGameName);
}

// top header
void drawTopHeader() {
  gb.display.setColor(BLACK);
  gb.display.fillRect(0, 0, LCDWIDTH, 7);
  gb.display.cursorX = 2;
  gb.display.cursorY = 1;
  gb.display.setColor(WHITE);
  gb.display.fillRect(0, 7, LCDWIDTH, 1);
}

void printTopHeader(const __FlashStringHelper* text) {
  drawTopHeader();
  printCentered(text);
}

void printTopHeader(char* text) {
  drawTopHeader();
  printCentered(text);
}

// bottom header
void drawBottomheader() {
  gb.display.setColor(BLACK);
  gb.display.fillRect(0, LCDHEIGHT - 7, LCDWIDTH, 7);
  gb.display.cursorX = 2;
  gb.display.cursorY = 42;
  gb.display.setColor(WHITE);
  gb.display.fillRect(0, LCDHEIGHT-8, LCDWIDTH, 1);
}

void printBottomHeader(const __FlashStringHelper* text) {
  drawBottomheader();
  printCentered(text);
}

void printBottomHeader(char* text) {
  drawBottomheader();
  printCentered(text);
}


void printCentered(const __FlashStringHelper* text){
  gb.display.cursorX = (LCDWIDTH / 2) - (strlen_PF((unsigned long) text) * gb.display.fontSize * gb.display.fontWidth / 2);
  gb.display.print(text);
}

void printCentered(char* text){
  gb.display.cursorX = (LCDWIDTH / 2) - (strlen(text) * gb.display.fontSize * gb.display.fontWidth / 2);
  gb.display.print(text);
}

boolean drawBitmapFromSD(byte x, byte y, byte w, byte h, char* fileName, unsigned int bitmapOffset) {
  if (file.open(fileName, O_READ)) {
    byte byteWidth = (w + 7) / 8;
    byte bufferHeight = BUFFER_SIZE / byteWidth;
    file.seekSet(bitmapOffset);
    for (byte j = 0; j < h; j++) {
      if (j % bufferHeight == 0) {                                //end of the buffer reached
        byte lengthToRead = byteWidth * min(bufferHeight, h - j); //either read until the buffer is full or the end of the picture is reached
        if (file.read(buffer, lengthToRead) < lengthToRead) {    //read the next part of the picture
          file.close();                                           //can't read the file
          return false;
        }
      }
      for (byte i = 0; i < w; i++) {
        if (buffer[((j % bufferHeight) * byteWidth + i / 8)] & (B10000000 >> (i % 8))) {
          gb.display.drawPixel(x + i, y + j);
        }
      }
    }
  } else {
    return false; //can't open file
  }
  file.close();
  return true;
}


boolean drawSlide(unsigned int slideNumber) {
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,8,LCDWIDTH,LCDHEIGHT-16);
  gb.display.setColor(BLACK);
  strcpy(completeName, thisPageFiles[cursorPos]);
  strcat(completeName, ".INF");
  return drawBitmapFromSD(0, 8, SLIDE_W, SLIDE_H, completeName, SLIDEOFFSET + (SLIDE_LENGTH * slideNumber));
}
