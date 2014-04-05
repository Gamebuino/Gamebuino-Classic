#define PAUSEMENULENGTH 6
char* PROGMEM pauseMenu[PAUSEMENULENGTH] = {
  "Resume",
  "Restart",
  "High scores",
  "Volume",
  "System info",
  "Change Game"
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
        /*case 3: //music ON/OFF
         if (gb.sound.isPlaying(1)) {
         gb.sound.stop();
         gb.popup("Music is OFF", 30);
         } 
         else {
         gb.sound.play(tetris_main1, 1);
         gb.sound.play(tetris_main2, 2);
         gb.sound.play(tetris_drum, 3);
         gb.sound.setLooping(1, true);
         gb.sound.setLooping(2, true);
         gb.sound.setLooping(3, true);
         gb.popup("Music is ON", 30);
         }
         break;*/
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
            gb.display.setTextColor(BLACK);
            gb.display.print(F("Bat:"));
            gb.display.print(gb.battery.getVoltage());
            gb.display.println(F("mV"));

            gb.display.print(F("Bat lvl:"));
            gb.display.print(gb.battery.getLevel());
            gb.display.println(F("/3"));

            gb.display.print(F("Light:"));
            gb.display.println(gb.backlight.ambientLight);

            gb.display.print(F("Backlight:"));
            gb.display.println(gb.backlight.backlightValue);

            gb.display.print(F("Volume:"));
            gb.display.print(gb.sound.getGlobalVolume());
            gb.display.print(F("/"));
            gb.display.println(VOLUME_GLOBAL_MAX);
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
      gb.display.setTextColor(BLACK,BLACK);
      gb.display.println(F("HIGH SCORES"));
      gb.display.setTextWrap(false);
      gb.display.setCursor(0,FONTHEIGHT);
      for(byte thisScore=0; thisScore<RANKMAX; thisScore++){
        if(highscore[thisScore]==0)
          break;
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




