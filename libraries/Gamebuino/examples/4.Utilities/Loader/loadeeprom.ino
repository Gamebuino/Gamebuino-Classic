void cleaneeprom(int i) {
  printBottomHeader(F("Cleaning EEPROM"));
  gb.display.update();
  for (; i < 1024; i++) {
    if (EEPROM.read(i))
      EEPROM.write(i, 0);
  }
}
void loadeeprom() {
  strcpy(completeName, nextGameName);
  for (byte i = 0; i < 8; i++) {
    if (completeName[i] == ' ')
      completeName[i] = '\0';
  }
  strcat(completeName, ".SAV");
  int i = 0;
  if (file.open(completeName, O_READ))
  {
    printBottomHeader(F("Loading saved game"));
    gb.display.update();
    word result=0;
    int k = 0;
    do {
      k = file.read(buffer, BUFFER_SIZE);
      if (k > 0)
      {
        for(byte j = 0; j<k; j++){
          if(EEPROM.read(i)!=buffer[j]){
            EEPROM.write(i,buffer[j]);
          }
          i++;
          if (i >= 1024) {
            break;
          }
        }
      }
      else if (i == 0) {
        printBottomHeader(F("File empty"));
      }
    } while (k > 0);
    file.close();
  }
  else{
    printBottomHeader(F("No saved game"));
  }
  cleaneeprom(i); // if the file is empty or non-existing this'll also get run, i will be 0 --> win
}
