void loadeeprom(){
  strcpy(completeName, nextGameName);
  for(byte i=0; i<8; i++){
    if(completeName[i] == ' ')
      completeName[i] = '\0';
  }
  strcat(completeName, ".SAV");
  gb.display.print("Loading ");
  gb.display.println(completeName);
  gb.display.update();
  res=file.openFile(completeName, FILEMODE_TEXT_READ);
  if (res==NO_ERROR)
  {
    word result=0;
    byte i = 0;
    while ((result!=EOF) and (result!=FILE_IS_EMPTY))
    {
      result=file.readLn(buffer, BUFFER_SIZE+2);
      if (result!=FILE_IS_EMPTY)
      {
        for(byte j=0; j<BUFFER_SIZE; j+=2){
          EEPROM.write((i*BUFFER_SIZE+j)/2,(buffer[j] & 0xF0) | (buffer[j+1] & 0x0F));
        }
        i++;
      }
      else{
        gb.display.println(F("File empty"));
        gb.display.update();
      }
    }
    file.closeFile();
  }
  else{
    gb.display.println(F("Not found"));
    gb.display.println(F("Cleaning EEPROM")); 
    gb.display.update();
    for (int i = 0; i < 1024; i++){
      if(EEPROM.read(i))
        EEPROM.write(i, 0);
    }
  }
}
