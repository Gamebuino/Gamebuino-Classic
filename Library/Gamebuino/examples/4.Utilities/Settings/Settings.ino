#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

byte buffer[128];
unsigned token;
char currentGame[9];
char userName[USERNAME_LENGTH+1];
unsigned contrast;
unsigned backlightMin;
unsigned backlightMax;
unsigned lightMin;
unsigned lightMax;
unsigned volumeMax;
unsigned volumeDefault;
unsigned startMenuTimer;
unsigned batteryCritic, batteryLow, batteryMed, batteryFull;

#define MAINMENU_LENGTH 5
char* PROGMEM mainMenu[MAINMENU_LENGTH] = {
  "See all settings",
  "Change settings",
  "Default settings",
  "Erase settings",
  "Save and Exit"
};

void setup(){
  gb.begin(F("Change settings"));
  if(!gb.settingsAvailable()){
    restoreSettings();
    saveSettings();
  }
  readSettings();
}

void loop(){
  switch(gb.menu(mainMenu,MAINMENU_LENGTH)){
  case 0: //display settings
    seeAllSettings();
    break;
  case 1: //change settings
    changeSettings();
    break;
  case 2: //default settings
    restoreSettings();
    break;
  case 3: //erase settings
    eraseSettings();
    break;
  case 4: //save and exit
    saveSettings();
    gb.changeGame();
    break;
  default:
    break;
  }
}

void pressAtoContinue(){
  gb.display.setCursor(LCDWIDTH-2*FONTWIDTH,LCDHEIGHT-FONTHEIGHT);
  gb.display.print(F("\25\20"));
  gb.display.update();
  while(1){
    gb.buttons.update();
    if(gb.buttons.pressed(BTN_A))
      break;
    delay(20);
  }
  gb.display.clear();
  gb.display.update();
}

void readSettings(){
  for(byte i=0; i<128; i++){
    buffer[i] = pgm_read_byte(SETTINGS_PAGE+i);
  }

  token = pgm_read_word(SETTINGS_PAGE);
  for(byte i=0; i<9; i++){
    currentGame[i] = (char)pgm_read_byte(SETTINGS_PAGE+OFFSET_CURRENTGAME+i);
  }
  for(byte i=0; i<USERNAME_LENGTH; i++){
    userName[i] = (char)pgm_read_byte(SETTINGS_PAGE+OFFSET_USERNAME+i);
  }
  contrast = pgm_read_byte(SETTINGS_PAGE+OFFSET_CONTRAST);
  backlightMin = pgm_read_byte(SETTINGS_PAGE+OFFSET_BACKLIGHT_MIN);
  backlightMax = pgm_read_byte(SETTINGS_PAGE+OFFSET_BACKLIGHT_MAX);
  lightMin = pgm_read_word(SETTINGS_PAGE+OFFSET_LIGHT_MIN);
  lightMax = pgm_read_word(SETTINGS_PAGE+OFFSET_LIGHT_MAX);
  volumeMax = pgm_read_byte(SETTINGS_PAGE+OFFSET_VOLUME_MAX);
  volumeDefault = pgm_read_byte(SETTINGS_PAGE+OFFSET_VOLUME_DEFAULT);
  startMenuTimer = pgm_read_byte(SETTINGS_PAGE+OFFSET_START_MENU_TIMER);
  batteryCritic = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_CRITIC);
  batteryLow = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_LOW);
  batteryMed = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_MED);
  batteryFull = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_FULL);
}

void saveSettings(){
  *(unsigned*)buffer = token;
  strcpy((char*)(&buffer[OFFSET_USERNAME]),userName);
  buffer[OFFSET_CONTRAST] = contrast;
  buffer[OFFSET_BACKLIGHT_MIN] = backlightMin;
  buffer[OFFSET_BACKLIGHT_MAX] = backlightMax;
  *(unsigned*)(&buffer[OFFSET_LIGHT_MIN]) = lightMin;
  *(unsigned*)(&buffer[OFFSET_LIGHT_MAX]) = lightMax;
  buffer[OFFSET_VOLUME_MAX] = volumeMax;
  buffer[OFFSET_VOLUME_DEFAULT] = volumeDefault;
  buffer[OFFSET_START_MENU_TIMER] = startMenuTimer;
  *(unsigned*)(&buffer[OFFSET_BATTERY_CRITIC]) = batteryCritic;
  *(unsigned*)(&buffer[OFFSET_BATTERY_LOW]) = batteryLow;
  *(unsigned*)(&buffer[OFFSET_BATTERY_MED]) = batteryMed;
  *(unsigned*)(&buffer[OFFSET_BATTERY_FULL]) = batteryFull;

  write_flash_page (SETTINGS_PAGE, buffer);
}

void eraseSettings(){
  memset(buffer, 0, 128);
  write_flash_page (SETTINGS_PAGE, buffer);

  gb.display.clear();
  gb.display.update();
  gb.display.persistence = true;
  gb.display.println(F("Settings page erased."));
  pressAtoContinue();
}




