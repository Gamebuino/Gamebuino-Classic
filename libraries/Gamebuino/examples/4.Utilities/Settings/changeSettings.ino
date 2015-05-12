//used in every menu:
const char strSave[] PROGMEM =            "Save";

///////////////////////////////////// SETTINGS MENU
#define SETTINGSMENU_LENGTH 6
const char strDisplay[] PROGMEM =  "Display";
const char strPlayerName[] PROGMEM =  "Player name";
const char strSound[] PROGMEM = "Sound";
const char strInterface[] PROGMEM =   "Interface";
const char strBattery[] PROGMEM =   "Battery";
const char* const settingsMenu[SETTINGSMENU_LENGTH] PROGMEM = {
  strDisplay,
  strPlayerName,
  strSound,
  strInterface,
  strBattery,
  strSave
};

///////////////////////////////////// DISPLAY MENU
#define DISPLAYMENU_LENGTH 6
const char strContrast[] PROGMEM =  "Contrast";
const char strBacklightMin[] PROGMEM =  "Backlight min";
const char strBacklightMax[] PROGMEM =  "Backlight max";
const char strAmbiantMin[] PROGMEM =  "Ambiant min";
const char strAmbiantMax[] PROGMEM =  "Ambiant max";
const char* const displayMenu[DISPLAYMENU_LENGTH] PROGMEM = {
  strContrast,
  strBacklightMin,
  strBacklightMax,
  strAmbiantMin,
  strAmbiantMax,
  strSave
};

///////////////////////////////////// SOUND MENU
#define SOUNDMENU_LENGTH 3
const char strDefaultVolume[] PROGMEM =  "Default volume";
const char strMaxVolume[] PROGMEM =  "Max volume";
const char* const soundMenu[SOUNDMENU_LENGTH] PROGMEM = {
  strDefaultVolume,
  strMaxVolume,
  strSave
};

///////////////////////////////////// INTERFACE MENU
#define INTERFACEMENU_LENGTH 2
const char strStartDuration[] PROGMEM =  "Start menu duration";
const char* const interfaceMenu[INTERFACEMENU_LENGTH] PROGMEM = {
  strStartDuration,
  strSave
};

///////////////////////////////////// BATTERY MENU
#define BATTERYMENU_LENGTH 5
const char strCritic[] PROGMEM =  "Critic voltage";
const char strLow[] PROGMEM =  "Low voltage";
const char strMed[] PROGMEM =  "Med voltage";
const char strHigh[] PROGMEM =  "High voltage";
const char* const batteryMenu[BATTERYMENU_LENGTH] PROGMEM = {
  strCritic,
  strLow,
  strMed,
  strHigh,
  strSave
};

boolean changeValue(int &val, int minVal, int maxVal, const __FlashStringHelper* text){
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
  
  gb.display.cursorX = 0;
  gb.display.cursorY = LCDHEIGHT-gb.display.fontHeight;
  gb.display.print(F("\21"));
  gb.display.print(val);
  gb.display.println(F("\20 \25:Accept"));
  
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.println(text);
  return(false);
}

void changeSettings(){
  while(1){
    switch(gb.menu(settingsMenu, SETTINGSMENU_LENGTH)){
    case 0: //display
      displaySettings();
      break;
    case 1: //username
      gb.keyboard(userName,USERNAME_LENGTH+1);
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
    case 5: //save
      saveSettings();
      return;
      break;
    default:
      return;
      break;
    }
  }
}

void displaySettings(){
  while(1){
    switch(gb.menu(displayMenu, DISPLAYMENU_LENGTH)){
    case 0: //contrast
      gb.setFrameRate(42);
      while(1){
        if(gb.update()){
          if(changeValue(contrast, 30, 80, F("Adjust constrasts")))
            break;
          gb.display.cursorX = 0;
          gb.display.cursorY = 20;
          gb.display.fillRect((gb.frameCount*2)%LCDWIDTH,8,3,3);
          gb.display.fillRect((gb.frameCount)%LCDWIDTH,8+4,3,3);
          gb.display.fillRect((gb.frameCount/4)%LCDWIDTH,8+2*4,3,3);
          gb.display.fillRect(0,24,16,16);
          gb.display.setColor(GRAY);
          gb.display.fillRect(17,24,16,16);
          gb.display.setColor(BLACK);
          gb.display.drawRect(34,24,16,16);
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
          if(changeValue(lightMax, 0, 1024, F("Max ambient light \n(when it's bright)\nassociated with\nmax backlight")))
            break;
          gb.display.print(F("\nCurrent ambient:"));
          gb.display.print(gb.backlight.ambientLight);
        }
      }
      gb.backlight.automatic = true;
      break;
    case 5: //save
      saveSettings();
      return;
      break;
    default:
      return;
      break;
    }
  }
}


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
          if(changeValue(volumeMax, 0, VOLUME_GLOBAL_MAX, F("Max volume:"))){
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
    case 2: //save
      saveSettings();
      return;
      break;
    default:
      return;
      break;
    }
  }
}


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
    case 1: //save
      saveSettings();
      return;
      break;
    default:
      return;
      break;
    }
  }
}

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
    case 4: //save
      saveSettings();
      return;
      break;
    default:
      return;
      break;
    }
  }
}













