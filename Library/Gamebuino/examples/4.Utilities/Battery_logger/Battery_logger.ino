#include <tinyFAT.h> //requires the tinyFAT library. You can download it here : http://www.henningkarlsen.com/electronics/library.php?id=37
#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

char buffer[7];

void setup(){
  gb.begin(F("Battery logger"));
  gb.backlight.automatic = false;
  gb.backlight.set(0);
  gb.display.clear();
  gb.display.persistence = true;

  if (file.initFAT() == NO_ERROR){
    gb.display.println(F("SD init OK"));
    gb.display.update();
  }
  else{
    gb.display.clear();
    gb.display.print(F("No SD card"));
    gb.display.update();
    while(1);
  }
  delay(1000);

  if(!file.exists("BATTERY.TXT")){
    file.create("BATTERY.TXT");
    gb.display.println(F("BATTERY.TXT created"));
  }

  if(file.openFile("BATTERY.TXT", FILEMODE_TEXT_WRITE) == NO_ERROR){
    gb.display.println(F("BATTERY.TXT opened"));
    gb.display.update();
    file.writeLn("I will log battery voltage every 10 minutes");
  }
  else{
    gb.display.clear();
    gb.display.println(F("Can't open BATTERY.TXT"));
    gb.display.update();
    while(1);
  }
  delay(1000);
}

void loop(){
  if(gb.update()){
    if((gb.frameCount%(20*60*10)) == 64){ //every 10 minutes
      itoa(gb.battery.voltage, buffer, 10);
      file.writeLn(buffer);
    }
    if((gb.frameCount%20) == 0){
      gb.display.clear();

      gb.display.print(gb.battery.voltage);
      gb.display.println("mV");

      gb.display.println(F("Running for"));
      gb.display.print(gb.frameCount/20/60/60);
      gb.display.print(":");
      gb.display.print((gb.frameCount/20/60)%60);
      gb.display.print("\"");
      gb.display.println((gb.frameCount/20)%60);
    }
  }
}










