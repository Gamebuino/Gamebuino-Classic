void restoreSettings(){
  token = 0xC001;
  strcpy(userName,"John Smith");
  contrast = SCR_CONTRAST ;
  backlightMin = BACKLIGHT_MIN;
  backlightMax = BACKLIGHT_MAX;
  lightMin = AMBIENTLIGHT_MIN;
  lightMax = AMBIENTLIGHT_MAX;
  volumeMax = VOLUME_GLOBAL_MAX;
  volumeDefault = VOLUME_GLOBAL_MAX;
  startMenuTimer = START_MENU_TIMER;
  batteryCritic = BAT_LVL_CRITIC;
  batteryLow = BAT_LVL_LOW;
  batteryMed = BAT_LVL_MED;
  batteryFull = BAT_LVL_FULL;

  gb.display.clear();
  gb.display.update();
  gb.display.persistence = true;
  gb.display.println(F("Default settings\nrestored."));
  pressAtoContinue();
}
