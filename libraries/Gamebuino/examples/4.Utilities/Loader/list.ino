void getFileExt(){
  byte where = 0;
  byte j = 0;
  for(byte i = 0;i < 13;i++){
    if((where == 0 && completeName[i] == '.') || (where == 1 && completeName[i] != '.')){
      where++;
    }
    if(where == 2){
      fileExt[j++] = completeName[i];
      if(j == 3){
        break;
      }
    }
  }
  fileExt[3] = '\0';
}

bool doDispFile(){
  file.getSFN(completeName);
  getFileExt();
  return strstr(fileExt,"HEX") && !strstr(completeName,"LOADER.HEX"); // we want to display a file if it is a HEX file and /not/ the loader
}
const byte defaultPic[] PROGMEM = {15,13, // default icon....this one sucks.
  0B00000000,0B00000000,
  0B00001111,0B00000000,
  0B00010000,0B00000000,
  0B00010000,0B00000000,
  0B00010011,0B00000000,
  0B00010001,0B01111000,
  0B00001111,0B01000100,
  0B00000000,0B01000100,
  0B00000000,0B01011000,
  0B00000000,0B01000100,
  0B00000000,0B01000100,
  0B00000000,0B01111000,
  0B00000000,0B00000000
};
void drawCursorBox(byte pos){ // draw the cursor box in whatever color is currently set
  gb.display.drawFastVLine(16*(pos%PAGEWIDTH),14*(pos/PAGEWIDTH) + 1,13);
  gb.display.drawFastVLine(16*(pos%PAGEWIDTH) + 16,14*(pos/PAGEWIDTH) + 1,13);
  gb.display.drawFastHLine(16*(pos%PAGEWIDTH),14*(pos/PAGEWIDTH),17);
  gb.display.drawFastHLine(16*(pos%PAGEWIDTH),14*(pos/PAGEWIDTH) + 14,17);
}
//bool filesDisplayed[PAGELENGTH]; // array of which files on the screens have an INF file

void updateCursor(){
  flashCounter = 0;
  gb.display.setColor(BLACK);
  gb.display.fillRect(0,42,84,6);
  if(oldCursorPos < PAGELENGTH){ // we need to delete the old cursor
    drawCursorBox(oldCursorPos);
  }
  gb.display.setColor(WHITE);
  drawCursorBox(cursorPos);
  
  oldCursorPos = cursorPos;
  gb.display.cursorX = 1;
  gb.display.cursorY = 43;
  strcpy(completeName,thisPageFiles[cursorPos]);
  strcat(completeName, ".INF");
  bool displayed = false;
  if(file.open(completeName,O_READ)){
    if(file.read(buffer, 1 + (2*13) + 22) >= 1 + (2*13) + 22 && buffer[0] == 0x01){
      buffer[1 + (2*13) + 21] = '\0';
      displayed = true; // this file has an INF file!
      gb.display.print(buffer + 1 + (2*13));
    }
  }
  if(!displayed){
    strcpy(completeName,thisPageFiles[cursorPos]);
    strcat(completeName, ".HEX");
    buffer[21] = '\0';
    file.open(completeName,O_READ);
    if(file.isLFN()){
      file.getName(buffer,21);
      gb.display.print(buffer);
    }else{
      gb.display.print(thisPageFiles[cursorPos]);
    }
  }
  file.close();
  gb.sound.playTick();
}

void updateList(){
  gb.display.clear();
  
  // draw the grid
  for(byte i = 0;i < 6;i++){
    gb.display.drawFastVLine(i*16,0,48);
  }
  for(byte i = 0;i < 4;i++){
    gb.display.drawFastHLine(0,i*14,84-3);
  }
  
  // loop throguh until we reached so many HEX files so that our page is next, paying attention that not every file on the sd card will be displayed
  int numFiles = 0;
  sd.chdir('/');
  
  filesOnPage = 0;
  while(file.openNext(sd.vwd(),O_READ)){
    if(doDispFile()){
      if(filesOnPage > 0 || (numFiles/PAGELENGTH == selectedPage)){
        byte k = 0; // here we grab the files of the page we are on
        for(;k < 8;k++){ // time to grab the filename, as we did a doDispFile() it is loaded in completeName
          if(completeName[k] == '.'){
            thisPageFiles[filesOnPage][k] = '\0';
            break;
          }
          thisPageFiles[filesOnPage][k] = completeName[k];
        }
        thisPageFiles[filesOnPage][++k] = '\0';
        //filesDisplayed[filesOnPage] = false; // easy way to set that whole array to false
        filesOnPage++;
        if(filesOnPage >= PAGELENGTH){
          file.close();
          break;
        }
      }else{
        numFiles++; // we need it to calc on which page we are
      }
    }
    file.close();
  }

  
  gb.display.setColor(BLACK);
  
  if(numberOfPages > 1){ //if there are several pages we draw the scrollbar
    //gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT);
    gb.display.fillRect(LCDWIDTH-3, selectedPage*(LCDHEIGHT-6)/numberOfPages, 3, 1+(LCDHEIGHT-6)/numberOfPages);
  }
  
  if(cursorPos > filesOnPage-1){
    cursorPos = filesOnPage-1;
  }
  
  for(byte k = 0;k < filesOnPage;k++){
    strcpy(completeName,thisPageFiles[k]);
    strcat(completeName, ".INF");
    bool displayed = false;
    if(file.open(completeName,O_READ)){
      if(file.read(buffer, 1 + (2*13) + 22) >= 1 + (2*13) + 22 && buffer[0] == 0x01){
        displayed = true; // this file has an INF file!
        byte x = 16*(k%PAGEWIDTH) + 1;
        byte y = 14*(k/PAGEWIDTH) + 1;
        byte w = 15;
        byte h = 13;
        byte i, j, byteWidth = (w + 7) / 8;
        for (j = 0; j < h; j++) {
          for (i = 0; i < w; i++) {
            if (buffer[1 + (j * byteWidth + i / 8)] & (B10000000 >> (i % 8))) {
              gb.display.drawPixel(x + i, y + j);
            }
          }
        }
      }
      file.close();
    }
    if(!displayed){
      gb.display.drawBitmap(16*(k%PAGEWIDTH) + 1,14*(k/PAGEWIDTH) + 1,defaultPic);
    }
  }
  
  oldCursorPos = PAGELENGTH;
  updateCursor();
}
