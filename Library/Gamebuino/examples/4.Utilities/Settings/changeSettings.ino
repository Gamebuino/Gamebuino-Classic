boolean changeValue(unsigned &val, unsigned minVal, unsigned maxVal, const __FlashStringHelper* text){
  if(gb.buttons.pressed(BTN_A)||gb.buttons.pressed(BTN_B)){
    gb.sound.playOK();
    return true;
  }
  if(gb.buttons.repeat(BTN_RIGHT,70/(gb.buttons.timeHeld(BTN_RIGHT)))){
    val++;
    if(gb.buttons.timeHeld(BTN_RIGHT) > 60)
      val++;
    gb.sound.playTick();
  }
  if(gb.buttons.repeat(BTN_LEFT,70/(gb.buttons.timeHeld(BTN_LEFT)))){
    val--;
    if(gb.buttons.timeHeld(BTN_LEFT) > 60)
      val--;
    gb.sound.playTick();
  }
  if (val > maxVal) val=minVal;
  if (val < minVal) val=maxVal;
  gb.display.setCursor(0,LCDHEIGHT-FONTHEIGHT);
  gb.display.print(F("\21"));
  gb.display.print(val);
  gb.display.println(F("\20 \25:Accept"));
  gb.display.setCursor(0,0);
  gb.display.println(text);
  return(false);
}

#define SETTINGSMENU_LENGTH 6
char* PROGMEM settingsMenu[SETTINGSMENU_LENGTH] = {
  "Player name",
  "Display",
  "Sound",
  "Interface",
  "Battery",
  "Save and Exit",
};

void changeSettings(){
  while(1){
    switch(gb.menu(settingsMenu, SETTINGSMENU_LENGTH)){
    case 0: //display
      gb.keyboard(userName,USERNAME_LENGTH+1);
      break;
    case 1: //display
      displaySettings();
      break;
    case 2: //sound
      soundSettings();
      break;
    case 3: //interface
      interfaceSettings();
      break;
    case 4: //battery
      batterySettings();
      break;
    case 5: //save and exit
      saveSettings();
      gb.changeGame();
      break;
    default:
      return;
      break;
    }
  }
}

#define DISPLAYMENU_LENGTH 5
char* PROGMEM displayMenu[DISPLAYMENU_LENGTH] = {
  "Contrast",
  "Backlight min",
  "Backlight max",
  "Ambiant min",
  "Ambiant max",
};

void displaySettings(){
  while(1){
    switch(gb.menu(displayMenu, DISPLAYMENU_LENGTH)){
    case 0: //contrast
      gb.setFrameRate(41);
      while(1){
        if(gb.update()){
          if(changeValue(contrast, 50, 70, F("Adjust constrast to\nsee \"GRAY\" correctly\n")))
            break;
          gb.display.setCursor(0,20);
          gb.display.println("BLACK");
          if(gb.frameCount%2)
            gb.display.println("GRAY");
          gb.display.drawRect(gb.frameCount%LCDWIDTH,16,6,6);
          gb.display.drawRect((gb.frameCount/2)%LCDWIDTH,16+7,6,6);
          gb.display.drawRect((gb.frameCount/4)%LCDWIDTH,16+2*7,6,6);
          gb.display.setContrast(contrast);
        }
      }
      gb.setFrameRate(20);
      break;
    case 1: //backlight min
      while(1){
        gb.backlight.automatic = false;
        if(gb.update()){
          if(changeValue(backlightMin, 0, 255, F("Set the minimum\namount of backlight:\n")))
            break;
          gb.backlight.set(backlightMin);
        }
      }
      gb.backlight.automatic = true;
      break;
    case 2: //backlight max
      while(1){
        gb.backlight.automatic = false;
        if(gb.update()){
          if(changeValue(backlightMax, 0, 255, F("Set the maximum\namount of backlight:\n")))
            break;
          gb.backlight.set(backlightMax);
        }
      }
      gb.backlight.automatic = true;
      break;
    case 3: //ambient min
      while(1){
        if(gb.update()){
          gb.backlight.automatic = false;
          gb.backlight.set(backlightMax);
          if(changeValue(lightMin, 0, 1024, F("Min ambient light \n(when it's dark)\nassociated with\nmin backlight")))
            break;
          gb.display.print(F("\nCurrent ambient:"));
          gb.display.print(gb.backlight.ambientLight);
        }
      }
      gb.backlight.automatic = true;
      break;
    case 4: //ambient max
      while(1){
        if(gb.update()){
          gb.backlight.automatic = false;
          gb.backlight.set(backlightMin);
          if(changeValue(lightMin, 0, 1024, F("Max ambient light \n(when it's bright)\nassociated with\nmax backlight")))
            break;
          gb.display.print(F("\nCurrent ambient:"));
          gb.display.print(gb.backlight.ambientLight);
        }
      }
      gb.backlight.automatic = true;
      break;
    default:
      return;
      break;
    }
  }
}

#define SOUNDMENU_LENGTH 2
char* PROGMEM soundMenu[SOUNDMENU_LENGTH] = {
  "Default volume",
  "Max volume",
};

void soundSettings(){
  while(1){
    switch(gb.menu(soundMenu, SOUNDMENU_LENGTH)){
    case 0: //default volume
      while(1){
        if(gb.update()){
          if(changeValue(volumeDefault, 0, volumeMax, F("Default volume:"))){
            gb.sound.setVolume(volumeMax);
            break;
          }
          if(gb.buttons.repeat(BTN_LEFT,4)||gb.buttons.repeat(BTN_RIGHT,4)){
            gb.sound.setVolume(volumeDefault);
            gb.sound.playOK();
          }
        }
      }
      break;
    case 1: //max volume
      while(1){
        if(gb.update()){
          if(changeValue(volumeMax, 0, 3, F("Max volume:"))){
            gb.sound.setVolume(volumeMax);
            break;
          }
          if(gb.buttons.repeat(BTN_LEFT,4)||gb.buttons.repeat(BTN_RIGHT,4)){
            gb.sound.setVolume(volumeMax);
            gb.sound.playOK();
          }
        }
      }
      break;
    default:
      return;
      break;
    }
  }
}

#define INTERFACEMENU_LENGTH 1
char* PROGMEM interfaceMenu[INTERFACEMENU_LENGTH] = {
  "Start menu duration",
};

void interfaceSettings(){
  while(1){
    switch(gb.menu(interfaceMenu, INTERFACEMENU_LENGTH)){
    case 0:
      while(1){
        if(gb.update()){
          if(changeValue(startMenuTimer, 0, 255, F("Start menu duration\nin number of frames\n(20 frames/second)\n0 to disable menu\n255 to disable time\n"))){
            break;
          }
        }
      }
      break;
    default:
      return;
      break;
    }
  }
}

#define BATTERYMENU_LENGTH 4
char* PROGMEM batteryMenu[BATTERYMENU_LENGTH] = {
  "Critic voltage",
  "Low voltage",
  "Med voltage",
  "High voltage",
};

void batterySettings(){
  while(1){
    switch(gb.menu(batteryMenu, BATTERYMENU_LENGTH)){
    case 0:
      while(1){
        if(gb.update()){
          if(changeValue(batteryCritic, 0, 5000, F("Automatic shutdown\nbelow this value\n(in milliVolts)"))){
            break;
          }
          gb.display.print(F("\nCurrent voltage:"));
          gb.display.print(gb.battery.voltage);
        }
      }
      break;
    case 1:
      while(1){
        if(gb.update()){
          if(changeValue(batteryLow, 0, 5000, F("Battery alert below\nthis value\n(in milliVolts)"))){
            break;
          }
          gb.display.print(F("\nCurrent voltage:"));
          gb.display.print(gb.battery.voltage);
        }
      }
      break;
    case 2:
      while(1){
        if(gb.update()){
          if(changeValue(batteryMed, 0, 5000, F("Battery shown empty\nbelow this value\n(in milliVolts)"))){
            break;
          }
          gb.display.print(F("\nCurrent voltage:"));
          gb.display.print(gb.battery.voltage);
        }
      }
      break;
    case 3:
      while(1){
        if(gb.update()){
          if(changeValue(batteryFull, 0, 5000, F("Battery shown full\nabove this value\n(in milliVolts)"))){
            break;
          }
          gb.display.print(F("\nCurrent voltage:"));
          gb.display.print(gb.battery.voltage);
        }
      }
      break;
    default:
      return;
      break;
    }
  }
}













