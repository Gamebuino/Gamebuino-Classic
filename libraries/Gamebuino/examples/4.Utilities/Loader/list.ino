bool doDispFile(){
  return strstr(file.DE.fileext,"HEX") && !strstr(file.DE.filename,"LOADER"); // we want to display a file if it is a HEX file and /not/ the loader
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
bool filesDisplayed[PAGELENGTH]; // array of which files on the screens have an INF file

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
  if(filesDisplayed[cursorPos]){
    file.currFile.currentCluster = thisPageClusters[cursorPos]; // we stored the cluster later on, this is a LOT faster than searching for the file again
    file.currFile.fileSize = 1 + (2*13) + 22; // header, icon, text
    file.currFile.currentPos = 1 + (2*13); // so that we are at the text
    file.currFile.fileMode = FILEMODE_TEXT_READ;
    file.readLn(buffer,22);
    buffer[21] = '\0'; // make sure the string actually ends where desired
    gb.display.print(buffer);
  }else{
    gb.display.print(thisPageFiles[cursorPos]);
  }
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
  int thisFile = 0;
  int numFiles = 0;
  res = file.findFirstFile(&file.DE);
  while(res == NO_ERROR){
    if(numFiles/PAGELENGTH == selectedPage){
      break;
    }
    if(strstr(file.DE.fileext,"HEX")){
      numFiles++;
    }
    thisFile++;
    res = file.findNextFile(&file.DE);
  }

  if(numberOfPages > 1){ //if there are several pages we draw the scrollbar
    //gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT);
    gb.display.fillRect(LCDWIDTH-3, selectedPage*(LCDHEIGHT-6)/numberOfPages, 3, 1+(LCDHEIGHT-6)/numberOfPages);
  }
  
  filesOnPage = 0;
  
  // this is to create the array of the filenames of the files currently been displayed
  do {
    if(doDispFile()){ // we only add it if we should display it!
      strcpy(thisPageFiles[filesOnPage],file.DE.filename);
      filesDisplayed[filesOnPage] = false; // easy way to set that whole array to false
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
  
  gb.display.setColor(BLACK);
  
  // now it's time to loop through all the files again and check for INF files, so if they have a custom icon etc.
  res = file.findFirstFile(&file.DE);
  while(res == NO_ERROR){
    if(strstr(file.DE.fileext,"INF")){
      // we have an INF file
      for(byte k = 0;k < filesOnPage;k++){ // se we check if it is required for this page
        if(strstr(file.DE.filename,thisPageFiles[k])){
          // the INF file is actually on this page
          file.currFile.currentCluster = file.DE.startCluster; // we "opeen" the file, note that for file reading the currFile object actually does not need the file name at all
          file.currFile.fileSize = file.DE.fileSize;
          file.currFile.currentPos = 0;
          file.currFile.fileMode = FILEMODE_TEXT_READ;
          file.readLn(buffer, 2*13 + 1);
          
          if(buffer[0] == 0x01){ // check for version number, would make it easier to modify the INF files later on and still support old ones
            filesDisplayed[k] = true; // this file has an INF file!
            
            // and now we display the sprite, needs a custom routine as the data is actually in RAM and not in PROGMEM
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
            
            thisPageClusters[k] = file.DE.startCluster; // after that we save the cluster where it was so that we can quickly fetch the name if the cursor is on this icon
          }
        }
      }
    }
    res = file.findNextFile(&file.DE);
  }
  
  // now draw the default icon for those without an INF file
  for(byte i = 0;i < filesOnPage;i++){
    if(!filesDisplayed[i]){
      gb.display.drawBitmap(16*(i%PAGEWIDTH) + 1,14*(i/PAGEWIDTH) + 1,defaultPic);
    }
  }
  
  oldCursorPos = PAGELENGTH;
  updateCursor();
}
