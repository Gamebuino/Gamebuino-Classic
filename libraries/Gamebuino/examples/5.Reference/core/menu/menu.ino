#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

#define MENULENGTH 6
char* PROGMEM menu[MENULENGTH] = {
  "System info",
  "Change game"
};

void setup(){
  gb.begin(F("Menu example"));
}

void loop(){
  switch(gb.menu(menu, MENULENGTH)){
    case 0:
      displaySystemInfo();
      break;
    case 1:
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
    }
  }
}
