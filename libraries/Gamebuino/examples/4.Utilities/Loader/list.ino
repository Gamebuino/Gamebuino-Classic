void getFileExt() {
  byte where = 0;
  byte j = 0;
  for (byte i = 0; i < 13; i++) {
    if ((where == 0 && completeName[i] == '.') || (where == 1 && completeName[i] != '.')) {
      where++;
    }
    if (where == 2) {
      fileExt[j++] = completeName[i];
      if (j == 3) {
        break;
      }
    }
  }
  fileExt[3] = '\0';
}

bool doDispFile() {
  file.getSFN(completeName);
  getFileExt();
  return strstr(fileExt, "HEX") && !strstr(completeName, "LOADER.HEX"); // we want to display a file if it is a HEX file and /not/ the loader
}
void drawCursorBox(byte pos) { // draw the cursor box in whatever color is currently set
  gb.display.drawRect((pos % PAGE_W) * (ICON_W + 1), (pos / PAGE_W) * (ICON_H + 1), (ICON_W + 2), (ICON_H + 2));
}
//bool filesDisplayed[PAGELENGTH]; // array of which files on the screens have an INF file

void updateCursor() {
  gb.display.setColor(BLACK);
  if (oldCursorPos < PAGELENGTH) { // we need to delete the old cursor
    drawCursorBox(oldCursorPos);
  }
  gb.display.setColor(WHITE);
  drawCursorBox(cursorPos);

  oldCursorPos = cursorPos;
  strcpy(completeName, thisPageFiles[cursorPos]);
  strcat(completeName, ".INF");
  bool displayed = false;
  //loads the .inf file
  if (file.open(completeName, O_READ)) {
    if (file.read(buffer, HEADERSIZE) == HEADERSIZE && buffer[VERSIONOFFSET] == 0x01) {
      buffer[1 + NAMEOFFSET + NAMELENGTH] = '\0';
      displayed = true; // this file has an INF file!
      printBottomHeader(buffer + NAMEOFFSET);
    }
  }
  if (!displayed) {
    strcpy(completeName, thisPageFiles[cursorPos]);
    strcat(completeName, ".HEX");
    buffer[NAMELENGTH] = '\0';
    file.open(completeName, O_READ);
    if (file.isLFN()) {
      file.getName(buffer, NAMELENGTH);
      printBottomHeader(buffer);
    } else {
      printBottomHeader(thisPageFiles[cursorPos]);
    }
  }
  file.close();
  //check if there is a saved game
  strcpy(completeName, thisPageFiles[cursorPos]);
  strcat(completeName, ".SAV");
  if (file.open(completeName, O_READ)) {
    file.close();
    gb.display.setColor(WHITE);
    gb.display.fillRect(0, 40, 9, 8);
    gb.display.setColor(BLACK);
    gb.display.drawBitmap(0, 40, floppy8x8);
  }
  gb.sound.playTick();
}

void updateList() {
  gb.display.clear();
  gb.display.setColor(BLACK);
  // draw the grid
  for (byte i = 0; i < 6; i++) {
    gb.display.drawFastVLine(i * (ICON_W + 1), 0, LCDHEIGHT - 9);
  }
  for (byte i = 0; i < 4; i++) {
    gb.display.drawFastHLine(0, i * (ICON_H + 1), LCDWIDTH - 4);
  }

  // loop throguh until we reached so many HEX files so that our page is next,
  // paying attention that not every file on the sd card will be displayed
  int numFiles = 0;
  sd.chdir('/');

  filesOnPage = 0;
  while (file.openNext(sd.vwd(), O_READ)) {
    if (doDispFile()) {
      if (filesOnPage > 0 || (numFiles / PAGELENGTH == selectedPage)) {
        byte k = 0; // here we grab the files of the page we are on
        for (; k < 8; k++) { // time to grab the filename, as we did a doDispFile() it is loaded in completeName
          if (completeName[k] == '.') {
            thisPageFiles[filesOnPage][k] = '\0';
            break;
          }
          thisPageFiles[filesOnPage][k] = completeName[k];
        }
        thisPageFiles[filesOnPage][++k] = '\0';
        //filesDisplayed[filesOnPage] = false; // easy way to set that whole array to false
        filesOnPage++;
        if (filesOnPage >= PAGELENGTH) {
          file.close();
          break;
        }
      } else {
        numFiles++; // we need it to calc on which page we are
      }
    }
    file.close();
  }

  gb.display.setColor(BLACK);

  if (numberOfPages > 1) { //if there are several pages we draw the scrollbar
    //gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT);
    gb.display.drawRect(LCDWIDTH - 4, 0, 4, LCDHEIGHT - 9);
    gb.display.fillRect(LCDWIDTH - 3, 1 + selectedPage * (LCDHEIGHT - 11) / numberOfPages, 3, 1 + (LCDHEIGHT - 11) / numberOfPages);
  }

  if (cursorPos > filesOnPage - 1) {
    cursorPos = filesOnPage - 1;
  }

  //draw game icons
  for (byte k = 0; k < filesOnPage; k++) {
    strcpy(completeName, thisPageFiles[k]);
    strcat(completeName, ".INF");
    bool displayed = false;
    byte x = (ICON_W + 1) * (k % PAGE_W) + 1;
    byte y = (ICON_H + 1) * (k / PAGE_W) + 1;
    byte w = ICON_W;
    byte h = ICON_H;
    if ( !drawBitmapFromSD(x, y, w, h, completeName, ICONOFFSET)) {
      //print the file's name if there is no icon
      //print the game's name if there's no icon
      gb.display.cursorX = (ICON_W + 1) * (k % PAGE_W) + 3;
      gb.display.cursorY = (ICON_H + 1) * (k / PAGE_W) + 2;
      strncpy(halfName, thisPageFiles[k], 4);
      gb.display.print(halfName);
      gb.display.cursorX = (ICON_W + 1) * (k % PAGE_W) + 3;
      gb.display.cursorY = (ICON_H + 1) * (k / PAGE_W) + 8;
      strncpy(halfName, thisPageFiles[k] + 4, 4);
      gb.display.print(halfName);
    }
  }

  oldCursorPos = PAGELENGTH;
  updateCursor();
}
