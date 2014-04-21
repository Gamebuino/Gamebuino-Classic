#define PAUSEMENULENGTH 6
char* PROGMEM pauseMenu[PAUSEMENULENGTH] = {
  "Resume",
  "Restart",
  "High scores",
  "Volume",
  "System info",
  "Exit"
};

///////////////////////////////////// PAUSE
void pause(){
  //gb.enableDisplayBattery = true;
  gb.popup(F(""),0);
  while(true){
    if(gb.update()){
      switch(gb.menu(pauseMenu, PAUSEMENULENGTH)){
      case 0: //resume
        delay(100);
        return;
      case 1: //restart
        initGame();   
        return;
      case 2: //high scores
        displayHighScores();
        break;
      case 3: //Volume
        gb.adjustVolume();
        break;
      case 4: //System info
        while (1) {
          if (gb.update()) {
            if (gb.buttons.pressed(BTN_C)) {
              gb.sound.playCancel();
              break;
            }
            gb.display.setCursor(0, 0);
            gb.display.print(F("Bat:"));
            gb.display.print(gb.battery.voltage);
            gb.display.println(F("mV"));

            gb.display.print(F("Bat lvl:"));
            gb.display.print(gb.battery.level);
            gb.display.println(F("/4"));

            gb.display.print(F("Light:"));
            gb.display.println(gb.backlight.ambientLight);

            gb.display.print(F("Backlight:"));
            gb.display.println(gb.backlight.backlightValue);

            gb.display.print(F("Volume:"));
            gb.display.print(gb.sound.getVolume());
            gb.display.print(F("/"));
            gb.display.println(gb.sound.volumeMax);

            gb.display.print("Mobs:");
            gb.display.print(activeMobs);
            gb.display.print("/");
            gb.display.println(NUMMOBS);

            gb.display.print("Killed:");
            gb.display.println(kills);
          }
        }
        break;
      case 5: //change game
        gb.changeGame();
      default:
        return;
      }
    }
  }
}

///////////////////////////////////// DISPLAY HIGHSCORES
void displayHighScores(){
  while(true){
    if(gb.update()){
      gb.display.setCursor(9+random(0,2),0+random(0,2));
      gb.display.println(F("HIGH SCORES"));
      gb.display.setTextWrap(false);
      gb.display.setCursor(0,FONTHEIGHT);
      for(byte thisScore=0; thisScore<RANKMAX; thisScore++){
        if(highscore[thisScore]==0)
          gb.display.print('-');
        else
          gb.display.print(name[thisScore]);
        gb.display.setCursor(LCDWIDTH-3*FONTWIDTH,FONTHEIGHT+FONTHEIGHT*thisScore);
        gb.display.println(highscore[thisScore]);
      }            
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        break;
      }
    }
  }
}






