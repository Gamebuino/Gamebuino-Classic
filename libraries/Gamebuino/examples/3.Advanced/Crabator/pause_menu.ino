#define PAUSEMENULENGTH 5
const char strPlay[] PROGMEM = "Play";
const char strRestart[] PROGMEM = "Restart";
const char strHighScores[] PROGMEM = "High scores";
const char strSystemInfo[] PROGMEM = "System Info";
const char strMainMenu[] PROGMEM = "Main Menu";


const char* const pauseMenu[PAUSEMENULENGTH] PROGMEM = {
  strPlay,
  strRestart,
  strHighScores,
  strSystemInfo,
  strMainMenu
};

///////////////////////////////////// PAUSE
void pause(){
  while(true){
    if(gb.update()){
      switch(gb.menu(pauseMenu, PAUSEMENULENGTH)){
      case 0: //resume
        delay(100);
        gb.display.setFont(font3x5);
        play();
        gb.display.setFont(font5x7);
        gb.battery.show = true;
        break;
      case 1: //restart
        initGame();
        gb.display.setFont(font3x5);
        play();
        gb.display.setFont(font5x7);
        gb.battery.show = true;
        return;
      case 2: //high scores
        displayHighScores();
        break;
      case 3: //System info
        gb.display.setFont(font3x5);
        while (1) {
          if (gb.update()) {
            if (gb.buttons.pressed(BTN_C)) {
              gb.display.setFont(font5x7);
              gb.sound.playCancel();
              break;
            }
            //gb.display.setCursor(0, 0);
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
      case 4: //change game
        //gb.changeGame();
        gb.titleScreen(logo);
        break;
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
      gb.display.cursorX = 9+random(0,2);
      gb.display.cursorY = 0+random(0,2);
      gb.display.println(F("HIGH SCORES"));
      gb.display.textWrap = false;
      gb.display.cursorX = 0;
      gb.display.cursorY = gb.display.fontHeight;
      for(byte thisScore=0; thisScore<RANKMAX; thisScore++){
        if(highscore[thisScore]==0)
          gb.display.print('-');
        else
          gb.display.print(name[thisScore]);
        gb.display.cursorX = LCDWIDTH-3*gb.display.fontWidth;
        gb.display.cursorY = gb.display.fontHeight+gb.display.fontHeight*thisScore;
        gb.display.println(highscore[thisScore]);
      }
      if(gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)){
        gb.sound.playOK();
        break;
      }
    }
  }
}
