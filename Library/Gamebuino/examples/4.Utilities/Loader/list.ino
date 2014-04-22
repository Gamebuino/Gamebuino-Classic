void updateCursor(){
  gb.sound.playTick();
  gb.display.setColor(WHITE);
  gb.display.fillRect(0,0,FONTWIDTH,LCDHEIGHT);
  gb.display.setColor(BLACK, WHITE);
  gb.display.setCursor(0,FONTHEIGHT*(selectedFile%PAGELENGTH));
  gb.display.print("\x10");
}

void updateList(){
  gb.display.clear();
  if((selectedFile < thisFile) || (thisFile == 0)){ //when going to the previous page
    thisFile = 0;
    res = file.findFirstFile(&file.DE);
    while(res == NO_ERROR){ //go to the first HEX file
      if(!showAll && strstr(file.DE.fileext, "HEX")) break;
      res = file.findNextFile(&file.DE);
    }
    //iterate through previous pages
    for(byte thisPage = 0; thisPage < selectedFile/PAGELENGTH; thisPage++){
      //iterate through files of previous pages
      for(byte i = 0; i<PAGELENGTH; i++){
        file.findNextFile(&file.DE);
        if(showAll || strstr(file.DE.fileext, "HEX")){
          thisFile++;
        }
      }
    }
  }

  if(numberOfFiles > PAGELENGTH){ //if there is several pages
    gb.display.drawFastVLine(LCDWIDTH-2,0,LCDHEIGHT);
    gb.display.fillRect(LCDWIDTH-3, selectedPage*LCDHEIGHT/numberOfPages, 3, 1+LCDHEIGHT/numberOfPages);
  }

  while (1)
  {
    if(showAll || strstr(file.DE.fileext, "HEX")){
      gb.display.print(" ");
      gb.display.print(file.DE.filename);
      if(showAll){
        gb.display.print(".");
        gb.display.println(file.DE.fileext);
      }
      else{
        gb.display.println();
      }
      thisFile++;
    }
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





