void cleaneeprom(int i){
  gb.display.println(F("Cleaning EEPROM"));
    gb.display.update();
    for (; i < 1024; i++){
      if(EEPROM.read(i))
        EEPROM.write(i, 0);
    }
}
void loadeeprom(){
  strcpy(completeName, nextGameName);
  for(byte i=0; i<8; i++){
    if(completeName[i] == ' ')
      completeName[i] = '\0';
  }
  strcat(completeName, ".SAV");
  res=file.openFile(completeName, FILEMODE_TEXT_READ);
  if (res==NO_ERROR)
  {
    gb.display.print("Loading saved game\n");
    gb.display.println(completeName);
    gb.display.update();
    word result=0;
    int i = 0;
    while ((result!=EOF) and (result!=FILE_IS_EMPTY))
    {
      result=file.readLn(buffer, BUFFER_SIZE+2);
      if (result!=FILE_IS_EMPTY)
      {
        for(byte j = 0; j<BUFFER_SIZE; j+=2){
          EEPROM.write(i,(buffer[j] & 0xF0) | (buffer[j+1] & 0x0F));
          i++;
        }
      }
      else{
        gb.display.println(F("File empty"));
      }
    }
    cleaneeprom(i); // if the file is empty this'll also get run, i will be 0 --> win
    file.closeFile();
  }
  else{
    gb.display.println(F("No saved game"));
    cleaneeprom(0);
  }
}
