#define PAUSEMENULENGTH 5
char* PROGMEM pauseMenu[PAUSEMENULENGTH] = {
  "Resume",
  "Restart",
  "High scores",
  "Sound on/off",
  "System info"
};

///////////////////////////////////// PAUSE
void pause(){
  //gb.enableDisplayBattery = true;
  gb.popup("",0);
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
      case 3: //sound ON/OFF
        delay(100);
        if (gb.sound.getGlobalVolume())
          gb.sound.setGlobalVolume(0);
        else
          gb.sound.setGlobalVolume(3);
        gb.sound.playOK();
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
            gb.display.print("Bat:");
            gb.display.print(gb.battery.getVoltage());
            gb.display.println("mV");

            gb.display.print("Bat lvl:");
            gb.display.print(gb.battery.getLevel());
            gb.display.println("/3");

            gb.display.print("Light:");
            gb.display.println(gb.backlight.ambientLight);

            gb.display.print("Backlight:");
            gb.display.println(gb.backlight.backlightValue);
            
            gb.display.print("CPU load:");
            gb.display.print(gb.getCpuLoad());
            gb.display.println("%");
            
            gb.display.print("Volume:");
            gb.display.print(gb.sound.getGlobalVolume());
          }
        }
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
      gb.display.setCursor(9+random(0,2),0+random(0,2));
      gb.display.setTextColor(BLACK,BLACK);
      gb.display.println("HIGH SCORES");
      gb.display.setTextWrap(false);
      gb.display.setCursor(0,8);
      for(byte thisScore=0; thisScore<RANKMAX; thisScore++){
        if(highscore[thisScore]==0)
          break;
        for(byte i=0; i<NAMELENGTH; i++){
          gb.display.print(name[thisScore][i]);
        }
        gb.display.print(" ");
        gb.display.println(highscore[thisScore]);
      }            
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        break;
      }
    }
  }
}


