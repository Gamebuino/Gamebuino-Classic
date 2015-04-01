void Wizard(){
  adjustContrast();
  
  gb.display.setFont(font5x7);
  while(1){
    if(gb.update()){
      gb.display.println(F("Check buttons\n"));
      //buttons symbols :
      gb.display.println(F("\33\30\32\31\25\26\27"));

      for(byte i = 0; i < NUM_BTN; i++){
        if(gb.buttons.states[i] != 0){
          //draw a star for used buttons
          gb.display.print("\52");
        }
        else{
          //draw a space for idle buttons
          gb.display.print(" ");
        }
        //tick sound when any button is pressed
        if(gb.buttons.states[i] == 1){
          gb.sound.playTick();
        }
      }

      gb.display.print(F("\n\nHold \25"));
      for(byte i = 0; i < gb.buttons.timeHeld(BTN_A)/4; i++){
        gb.display.print("\20");
      }
      if(gb.buttons.held(BTN_A, 16)){
        break;
      }
    }
  }


  while(1){
    if(gb.update()){
      gb.display.print(F("Battery\n"));
      gb.display.print(gb.battery.voltage);
      gb.display.print("mV ");
      if(gb.battery.voltage < 3500){
        if((gb.frameCount % 16) < 8) gb.display.println('\7'); //blinking battery
        else gb.display.println('x');
        gb.display.cursorY = 40;
        gb.display.print(F("\25:Skip error"));
        if(gb.buttons.pressed(BTN_A)){
          gb.battery.thresholds[0] = 0;
          batteryCritic = 0;
          break;
        }
      }
      else{
        gb.display.println(F("OK"));
        gb.display.cursorY = 40;
        gb.display.print(F("\25:Accept"));
        if(gb.buttons.pressed(BTN_A)){
          break;
        }
      }
    }
  }

  while(1){
    if(gb.update()){
      gb.display.println(F("Backlight:"));
      gb.display.println(F("\52        \17"));
      gb.display.drawRect(8,9, 255/6,6);
      gb.display.fillRect(8,9, gb.backlight.backlightValue/6,6);
      gb.display.println(F("Light sensor:"));
      gb.display.println(F("\52        \17"));
      gb.display.drawRect(8,25, 1024/25,6);
      gb.display.fillRect(8,25, gb.backlight.ambientLight/25,6);

      gb.display.cursorY = 40;
      gb.display.print(F("\25:Accept"));
      if(gb.buttons.pressed(BTN_A)){
        break;
      }
    }
  }
  gb.display.setFont(font3x5);
}






