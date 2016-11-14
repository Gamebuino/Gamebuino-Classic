void saveeeprom(){

  boolean isEmpty = true;
  for(int i = 0; i< 1024; i++){
    if(EEPROM.read(i)){
      isEmpty = false;
      break;
    }
  }
  if(isEmpty){
    printBottomHeader(F("Nothing to be saved!"));
    return;
  }
  strcpy(completeName, nextGameName);
  strcat(completeName, ".SAV");
  
  printBottomHeader(F("Saving game"));
  
  /*
  gb.display.println(F("\n\25:yes \26:no"));
  gb.display.update();
  while(1){
    gb.buttons.update();
    if(gb.buttons.pressed(BTN_A)) break;
    if(gb.buttons.pressed(BTN_B)) return;
    delay(50);
  }*/



  
  //to ask confirmation before overwriting existing saves
  /*if(file.open(completeName,O_READ)){
    file.close();
    gb.display.println(F("Overwrite existing?"));
    gb.display.println(F("\25:yes \26:no"));
    gb.display.update();
    while(1){
      gb.buttons.update();
      if(gb.buttons.pressed(BTN_A)){
        //file.delFile(completeName);
        break;
      }
      if(gb.buttons.pressed(BTN_B)) return;
      delay(50);
    }
  }*/
  
  if (file.open(completeName,O_RDWR | O_CREAT))
  {
    for(byte i=0; i< 1024/BUFFER_SIZE; i++){
      for(byte j = 0; j<BUFFER_SIZE; j++){
        buffer[j] = EEPROM.read(i*BUFFER_SIZE+j);
      }
      file.write(buffer,BUFFER_SIZE);
    }
    file.close();
    printBottomHeader(F("Game loaded"));
    gb.display.update();
  }
  else{
    printBottomHeader(F("Error"));
  }
  /*gb.display.println(F("\25:continue"));
  gb.display.update();
  while(1){
    gb.buttons.update();
    if(gb.buttons.pressed(BTN_A)){
      break;
    }
    delay(50);
  }*/
}




