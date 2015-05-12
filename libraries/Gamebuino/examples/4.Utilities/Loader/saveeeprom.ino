void saveeeprom(){

  boolean isEmpty = true;
  for(int i = 0; i< 1024; i++){
    if(EEPROM.read(i)){
      isEmpty = false;
      break;
    }
  }
  if(isEmpty){
    gb.display.println(F("Nothing to be saved!"));
    return;
  }
  strcpy(completeName, nextGameName);
  strcat(completeName, ".SAV");
  
  gb.display.clear();
  gb.display.print(F("Saving EEPROM to\n"));
  gb.display.print(completeName);
  gb.display.print("\n");
  
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
    gb.display.print(completeName);
    gb.display.println(F(" saved"));
    gb.display.update();
  }
  else{
    gb.display.println(F("Error"));
    gb.display.update();
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




