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
  gb.display.clear();
  gb.display.print(F("Save EEPROM to\n"));
  gb.display.print(prevGameName);
  gb.display.println(F(".SAV?"));
  gb.display.println(F("\25:yes \26:no"));
  gb.display.update();
  while(1){
    gb.buttons.update();
    if(gb.buttons.pressed(BTN_A)) break;
    if(gb.buttons.pressed(BTN_B)) return;
    delay(50);
  }



  strcpy(completeName, prevGameName);
  strcat(completeName, ".SAV");
  if(file.exists(completeName)){
    gb.display.println(F("Overwrite existing?"));
    gb.display.println(F("\25:yes \26:no"));
    gb.display.update();
    while(1){
      gb.buttons.update();
      if(gb.buttons.pressed(BTN_A)){
        file.delFile(completeName);
        break;
      }
      if(gb.buttons.pressed(BTN_B)) return;
      delay(50);
    }
  }
  file.create(completeName);
  res=file.openFile(completeName, FILEMODE_TEXT_WRITE);
  if (res==NO_ERROR)
  {
    for(byte i=0; i< 1024/BUFFER_SIZE; i++){
      buffer[BUFFER_SIZE+1] = '\0';
      for(byte j = 0; j<BUFFER_SIZE; j+=2){
        buffer[j] = 0x0F | EEPROM.read((i*BUFFER_SIZE+j)/2);
        buffer[j+1] = 0xF0 |  EEPROM.read((i*BUFFER_SIZE+j)/2);
      }
      file.writeLn(buffer);
    }
    file.closeFile();
    gb.display.print(completeName);
    gb.display.println(F(" saved"));
    gb.display.update();
  }
  else{
    gb.display.println(F("Error"));
    gb.display.update();
  }
}



