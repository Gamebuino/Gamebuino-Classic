#define MENU_PLAY 0
#define MENU_DELETESAVE 1
#define MENU_DELETEGAME 2

#define MENU_SIZE 3

byte slideNumber;
byte currentSlide;
byte menuSelection;
boolean askingConfirmation;
byte scrollBarTimeLeft;

void displayGameScreen() {
  initDisplayGameScreen();

  while (1) {
    if (gb.update()) {
      if (gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)) {
        gb.sound.playTick();
        if (askingConfirmation) {
          askingConfirmation = false;
        } else {
          return;
        }
      }
      if (gb.buttons.pressed(BTN_A)) {
        gb.sound.playTick();
        switch (menuSelection) {
          case MENU_PLAY :
            loadSelectedFile();
            break;
          case MENU_DELETESAVE:
            askingConfirmation = true;
            break;
          case MENU_DELETEGAME:
            askingConfirmation = true;
            break;
        }
      }
      if (askingConfirmation && gb.buttons.held(BTN_A, 20)) {
        gb.sound.playTick();
        switch (menuSelection) {
          case MENU_DELETEGAME:
            strcpy(completeName, thisPageFiles[cursorPos]);
            strcat(completeName, ".INF");
            if (file.open(completeName, O_READ | O_WRITE)) {
              file.remove();
              file.close();
            }
            strcpy(completeName, thisPageFiles[cursorPos]);
            strcat(completeName, ".HEX");
            if (file.open(completeName, O_READ | O_WRITE)) {
              file.remove();
              file.close();
            }
            return;
          case MENU_DELETESAVE:
            strcpy(completeName, thisPageFiles[cursorPos]);
            strcat(completeName, ".SAV");
            if (file.open(completeName, O_READ | O_WRITE)) {
              file.remove();
              file.close();
            }
            initDisplayGameScreen();
            break;
        }
      }

      if (gb.buttons.pressed(BTN_UP) || (gb.buttons.states[BTN_UP] > 10 && gb.buttons.repeat(BTN_UP, 3))) {
        currentSlide += slideNumber - 1;
        currentSlide %= slideNumber;
        drawSlide(currentSlide);
        scrollBarTimeLeft = 12;
      } else {
        if (gb.buttons.pressed(BTN_DOWN) || (gb.buttons.states[BTN_DOWN] > 10 && gb.buttons.repeat(BTN_DOWN, 3)))  {
          currentSlide ++;
          currentSlide %= slideNumber;
          drawSlide(currentSlide);
          scrollBarTimeLeft = 12;
        }
      }



      if (gb.buttons.repeat(BTN_RIGHT, 3)) {
        menuSelection = (menuSelection + 1) % MENU_SIZE;
      } else {
        if (gb.buttons.repeat(BTN_LEFT, 3)) {
          menuSelection = (menuSelection + MENU_SIZE - 1) % MENU_SIZE;
        }
      }

      if (askingConfirmation) {
        printBottomHeader(F("Hold A to delete"));
      } else {
        switch (menuSelection) {
          case MENU_PLAY :
            printBottomHeader(F("\21 PLAY \20"));
            break;
          case MENU_DELETESAVE:
            printBottomHeader(F("\21 DELETE SAVE \20"));
            break;
          case MENU_DELETEGAME:
            printBottomHeader(F("\21 DELETE GAME \20"));
            break;
        }
      }

      if ((slideNumber > 1) && (scrollBarTimeLeft > 0)) {
        scrollBarTimeLeft --;
        if (!scrollBarTimeLeft) { //time to hide the scrollbar and redraw the bitmap
          gb.display.setColor(WHITE);
          gb.display.fillRect(LCDWIDTH - 5, 7, 5, LCDHEIGHT - 7 - 7);
          gb.display.setColor(BLACK);
          drawSlide(currentSlide);
        } else { //draw the scrollbar
          gb.display.setColor(WHITE);
          gb.display.fillRect(LCDWIDTH - 5, 7, 5, LCDHEIGHT - 7 - 7);
          gb.display.setColor(BLACK);
          gb.display.drawRect(LCDWIDTH - 4, 7, 1, LCDWIDTH - 4);
          gb.display.drawRect(LCDWIDTH - 1, 7, 1, LCDWIDTH - 1);
          gb.display.fillRect(LCDWIDTH - 3,
                              7 + currentSlide * (LCDHEIGHT - 7 - 7) / slideNumber,
                              3,
                              1 + max(3, (LCDHEIGHT - 7 - 7) / slideNumber));
        }
      }

      //draw a floppy if there is a saved game
      strcpy(completeName, thisPageFiles[cursorPos]);
      strcat(completeName, ".SAV");
      if (file.open(completeName, O_READ)) {
        file.close();
        gb.display.setColor(WHITE);
        gb.display.fillRect(0, 40, 9, 8);
        gb.display.setColor(BLACK);
        gb.display.drawBitmap(0, 40, floppy8x8);
      }

    }
  }
}

void initDisplayGameScreen() {

  gb.display.clear();
  gb.display.setColor(BLACK);

  slideNumber = 0;
  currentSlide = 0;
  scrollBarTimeLeft = 30;

  menuSelection = 0;
  askingConfirmation = false;

  strcpy(completeName, thisPageFiles[cursorPos]);
  strcat(completeName, ".INF");
  bool displayed = false;
  //loads the .inf file
  if (file.open(completeName, O_READ)) {
    if (file.read(buffer, HEADERSIZE) == HEADERSIZE && buffer[VERSIONOFFSET] == 0x01) {
      buffer[1 + NAMEOFFSET + NAMELENGTH] = '\0';
      displayed = true; // this file has an INF file!
      printTopHeader(buffer + NAMEOFFSET);
      slideNumber = buffer[SLIDENUMBEROFFSET];
    }
  }
  if (!displayed) {
    strcpy(completeName, thisPageFiles[cursorPos]);
    strcat(completeName, ".HEX");
    buffer[NAMELENGTH] = '\0';
    file.open(completeName, O_READ);
    if (file.isLFN()) {
      file.getName(buffer, NAMELENGTH);
      printTopHeader(buffer);
    } else {
      printTopHeader(thisPageFiles[cursorPos]);
    }
  }
  file.close();

  if (!drawSlide(currentSlide)) {
    gb.display.cursorY = 22;
    gb.display.cursorX = 3;
    gb.display.print(F(" - No description - "));
  }
}
