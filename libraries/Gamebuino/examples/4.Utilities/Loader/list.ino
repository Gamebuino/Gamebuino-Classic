const char muliplierChars[] PROGMEM = " kMG";

void updateCursor(){
  gb.sound.playTick();
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,gb.display.fontWidth,LCDHEIGHT);
  gb.display.setColor(BLACK, WHITE);
  gb.display.cursorX = 0;
  gb.display.cursorY = gb.display.fontHeight*(selectedFile%PAGELENGTH);
  gb.display.print("\x10");
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

    boolean isHex = false;
    if(strstr(file.DE.fileext,"HEX")){
      isHex = true;
    }

    if(isHex){
      gb.display.print(" ");
    }
    else{
      gb.display.print("*");
    }

    gb.display.print(file.DE.filename);
    gb.display.print(".");
    gb.display.print(file.DE.fileext);

    unsigned long size = file.DE.fileSize;
    byte multiplier = 0;
    while(1){
      if(size < 999){
        gb.display.print(" ");
        gb.display.print(size);
        if(multiplier){
          gb.display.print((char)pgm_read_byte(muliplierChars + multiplier));
        }
        gb.display.println('B');
        break;
      }
      size /= 1024;
      multiplier ++;
    }

    thisFile++;
    //open next file
    res = file.findNextFile(&file.DE);
    if(res != NO_ERROR){
      break;
    }
    //don't display next page
    if(thisFile > ((selectedFile/PAGELENGTH)*PAGELENGTH+PAGELENGTH-1))
      break;
  }
  updateCursor();
}
