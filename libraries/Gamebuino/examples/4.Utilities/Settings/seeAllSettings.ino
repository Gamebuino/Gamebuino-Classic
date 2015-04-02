void seeAllSettings(){
  gb.display.clear();
  gb.display.update();
  gb.display.persistence = true;

  gb.display.print(F("token:0x"));
  gb.display.println(token, HEX);

  gb.display.print(F("game:\""));
  gb.display.print(currentGame);
  gb.display.println(F("\""));

  gb.display.print(F("player:\""));
  gb.display.print(userName);
  gb.display.println(F("\""));

  gb.display.print(F("contrast:"));
  gb.display.println(contrast);

  gb.display.print(F("backlight min:"));
  gb.display.println(backlightMin);

  gb.display.print(F("backlight max:"));
  gb.display.println(backlightMax);

  gb.display.print(F("light min:"));
  gb.display.println(lightMin);

  gb.display.print(F("light max:"));
  gb.display.println(lightMax);

  pressAtoContinue();

  gb.display.print(F("volume max:"));
  gb.display.println(volumeMax);

  gb.display.print(F("volume default:"));
  gb.display.println(volumeDefault);

  gb.display.print(F("start menu timer:"));
  gb.display.println(startMenuTimer);

  gb.display.println(F("Battery levels (mV)"));

  gb.display.print(F("Critic:"));
  gb.display.println(batteryCritic);

  gb.display.print(F("Low:"));
  gb.display.println(batteryLow);

  gb.display.print(F("Med:"));
  gb.display.println(batteryMed);

  gb.display.print(F("High:"));
  gb.display.println(batteryFull);

  pressAtoContinue();

}
