bool doDispFile(){
  return strstr(file.DE.fileext,"HEX") && !strstr(file.DE.filename,"LOADER");
}
const byte defaultPic[] PROGMEM = {16,12,
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
  0B00000000,0B01111000
};
void updateCursor(){
  gb.sound.playTick();
  if(oldCursorPos < PAGELENGTH){ // we need to delete the old cursor
    gb.display.setColor(WHITE);
    gb.display.drawFastVLine(20*(oldCursorPos%4),oldCursorPos<4 ? 0 : 24,24);
    gb.display.drawFastVLine(20*(oldCursorPos%4) + 18,oldCursorPos<4 ? 0 : 24,24);
  }
  gb.display.setColor(BLACK);
  gb.display.drawFastVLine(20*(cursorPos%4),cursorPos<4 ? 0 : 24,24);
  gb.display.drawFastVLine(20*(cursorPos%4) + 18,cursorPos<4 ? 0 : 24,24);
  oldCursorPos = cursorPos;
}

void updateList(){
  gb.display.clear();
  int thisFile = 0;
  int numFiles = 0;
  res = file.findFirstFile(&file.DE);
  while(res == NO_ERROR){
    if(numFiles/8 == selectedPage){
      break;
    }
    if(strstr(file.DE.fileext,"HEX")){
      numFiles++;
    }
    thisFile++;
    res = file.findNextFile(&file.DE);
  }

  if(numberOfFiles > PAGELENGTH){ //if there are several pages
    gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT);
    gb.display.fillRect(LCDWIDTH-3, selectedPage*LCDHEIGHT/numberOfPages, 3, 1+LCDHEIGHT/numberOfPages);
  }
  
  filesOnPage = 0;
  
  do {
    if(doDispFile()){
      gb.display.cursorY = filesOnPage<4 ? 19 : 43;
      gb.display.cursorX = 20*(filesOnPage%4) + 2;
      strcpy(nextGameName,file.DE.filename);
      
      gb.display.print(nextGameName+4);
      
      nextGameName[4] = '\0';
      gb.display.cursorY = filesOnPage<4 ? 13 : 37;
      gb.display.cursorX = 20*(filesOnPage%4) + 2;
      gb.display.print(nextGameName);
      
     
      /*int counter = 0;
        res = file.findFirstFile(&file.DE);
        do{
          counter++;
          res = file.findNextFile(&file.DE);
        }while(counter != thisFile);*/
      
      strcpy(thisPageFiles[filesOnPage],file.DE.filename);
      
      filesOnPage++;
    }

    thisFile++;
    //open next file
    res = file.findNextFile(&file.DE);
    if(res != NO_ERROR){
      if(cursorPos > filesOnPage-1){
        cursorPos = filesOnPage-1;
      }
      break;
    }
    
  } while (filesOnPage < PAGELENGTH);
  
  file.closeFile();
  gb.display.cursorY = 0;
  gb.display.cursorX = 0;
  gb.display.setColor(BLACK);
  for(byte i = 0;i < filesOnPage;i++){
    strcpy(completeName,thisPageFiles[i]);
    byte x = 20*(i%4) + 2;
    byte y = i < 4 ? 0 : 24;
    for(byte i=0; i<8; i++){
      if(completeName[i] == ' ')
        completeName[i] = '\0';
    }
    strcat(completeName, ".ICO");
    if(file.openFile(completeName, FILEMODE_TEXT_READ)==NO_ERROR){
      res = file.readLn(buffer, 2*12 + 2);
      buffer[2*12 + 3] = '\0';
      if(res!=FILE_IS_EMPTY){
        byte w = buffer[0];
        byte h = buffer[1];
        byte i, j, byteWidth = (w + 7) / 8;
        for (j = 0; j < h; j++) {
            for (i = 0; i < w; i++) {
                if (buffer[2 + (j * byteWidth + i / 8)] & (B10000000 >> (i % 8))) {
                    gb.display.drawPixel(x + i, y + j);
                }
            }
        }
      }
      file.closeFile();
      
    }else{
      gb.display.drawBitmap(x,y,defaultPic);
    }
  }
  
  
  oldCursorPos = PAGELENGTH;
  updateCursor();
}
