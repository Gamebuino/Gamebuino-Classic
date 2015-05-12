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
  if (file.open(completeName, O_READ))
  {
    gb.display.print("Loading saved game\n");
    gb.display.println(completeName);
    gb.display.update();
    word result=0;
    int i = 0;
    int k = 0;
    do {
      k = file.read(buffer,BUFFER_SIZE);
      if (k > 0)
      {
        for(byte j = 0; j<k; j++){
          EEPROM.write(i,buffer[j]);
          i++;
          if(i >= 1024){
            break;
          }
        }
      }
      else if(i == 0){
        gb.display.println(F("File empty"));
      }
    } while (k > 0);
    cleaneeprom(i); // if the file is empty this'll also get run, i will be 0 --> win
    file.close();
  }
  else{
    gb.display.println(F("No saved game"));
    cleaneeprom(0);
  }
}
