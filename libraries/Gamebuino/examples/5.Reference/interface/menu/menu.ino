#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

////declare all the variables needed to make a menu
//number of items in the menu
#define MENULENGTH 2
//The different strings to put in the menu
//each string can be used in different menus
const char strSystemInfo[] PROGMEM = "Stystem info";
const char strChangeGame[] PROGMEM = "Change game";
//Put all the different items together in a menu (an array of strings actually)
const char* const menu[MENULENGTH] PROGMEM = {
  strSystemInfo,
  strChangeGame,
};

void setup(){
  gb.begin();
  gb.titleScreen(F("Menu example"));
}

void loop(){
  switch(gb.menu(menu, MENULENGTH)){
    case -1: //nothing selected
      gb.titleScreen(F("Menu example"));
      break;
    case 0: //display system info
      displaySystemInfo();
      break;
    case 1: //change game
      gb.changeGame();
      break;
    default:
      break;
  }
}

void displaySystemInfo(){
  while(1){
    if(gb.update()){
      if (gb.buttons.pressed(BTN_C)) {
        gb.sound.playCancel();
        return;
      }
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
    }
  }
}
