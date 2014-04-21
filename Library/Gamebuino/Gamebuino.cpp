/* 
 * File:   Gamebuino.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 5:39 PM
 */

#include "Gamebuino.h"
PROGMEM uint16_t startupSound[] = {0x1E, 0x10, 0x301E, 0x10, 0x602E, 0x602C, 0x602A, 0x6028, 0x6026, 0x6024, 0x6022, 0x80, 0x0000};
static unsigned char PROGMEM gamebuinoLogo[] =
{
  84,10, //width and height
  B00000011, B11100001, B10000001, B10000110, B01111111, B00111110, B00011000, B01101101, B10000011, B00001111, B00001111, 
  B00001110, B00000001, B10000011, B10000110, B01100000, B00110011, B00011000, B01101101, B11000011, B00011001, B10001111, 
  B00011000, B00000011, B11000011, B10001110, B01100000, B00110011, B00011000, B01101101, B11100011, B00110000, B11001111, 
  B00011000, B00000011, B11000011, B10011110, B01100000, B00110110, B00110000, B11001101, B11100011, B01100000, B11001111, 
  B00110000, B00000110, B11000111, B10011110, B01111110, B00111110, B00110000, B11001101, B10110011, B01100000, B11001111, 
  B00110000, B00001100, B11000110, B11110110, B01100000, B00110011, B00110000, B11011001, B10110110, B01100000, B11001111, 
  B00110011, B11001111, B11001100, B11110110, B01100000, B01100001, B10110000, B11011011, B00011110, B01100000, B11001111, 
  B00110000, B11011000, B01101100, B11100110, B11000000, B01100001, B10110000, B11011011, B00011110, B01100001, B10001111, 

  B00011001, B10011000, B01101100, B11000110, B11000000, B01100011, B10110001, B10011011, B00001110, B00110011, B00001111, 
  B00001111, B10110000, B01111000, B11000110, B11111111, B01111110, B00011111, B00011011, B00000110, B00011110, B00001111, 
};

static unsigned char PROGMEM startMenuIcons[] =
{
  16,28, //width and height
  B00111100, B00000000, 
  B01000010, B00001110, 
  B10010001, B00011011, 
  B10011001, B00010101, 
  B10011101, B00010001, 
  B10011001, B00010101, 
  B10010001, B00011111, 
  B01000010, B00001110, 

  B00111100, B00000000, 
  B00000000, B00000000, 
  B00000000, B00000000, 
  B00000000, B00001110, 
  B00000000, B00010111, 
  B00000000, B00010001, 
  B00000000, B00010101, 
  B00000000, B00010001, 

  B00000000, B00011111, 
  B00000000, B00001110, 
  B00000000, B00000000, 
  B00000000, B00000000, 
  B01111000, B00000000, 
  B10000100, B00001110, 
  B11111111, B11011111, 
  B11000000, B01010001, 

  B11000000, B01010111, 
  B10000000, B10010001, 
  B10000000, B10011111, 
  B11111111, B10001110, 
};

Gamebuino::Gamebuino() {
}

void Gamebuino::begin(const __FlashStringHelper*  name, const uint8_t *logo) {
    timePerFrame = 50;
    nextFrameMillis = 0;
    frameCount = 0;
	frameEndMicros = 1;
	startMenuTimer = 255;

	readSettings();
	
    backlight.begin();
    buttons.begin();
    battery.begin();
    sound.begin();
    display.begin(SCR_CLK, SCR_DIN, SCR_DC, SCR_CS, SCR_RST);
	
    backlight.set(BACKLIGHT_MAX);
	display.persistence = true;
	
	if(startMenuTimer){
	#if LCDWIDTH == LCDWIDTH_NOROT
		display.drawBitmap(0,0, gamebuinoLogo);
		if(logo){
			display.drawBitmap(0, 12+FONTHEIGHT, logo);
		}
		display.setCursor(0,12);
	#else
		display.drawBitmap(7,0, gamebuinoLogo);
		display.drawBitmap(-41,12,gamebuinoLogo);
		if(logo){
			display.drawBitmap(0, 24+FONTHEIGHT, logo);
		}
		display.setCursor(0,24); 
	#endif
	display.print(name);
	display.drawBitmap(LCDWIDTH-16, LCDHEIGHT-28, startMenuIcons);
	display.drawChar(LCDWIDTH-5-2*FONTWIDTH,LCDHEIGHT-17,'\23', 1); //speaker logo

    sound.play(startupSound, 0);
	while(1){
		if(update()){
			if(buttons.pressed(BTN_B)){
				sound.setVolume(0);
			}
			
			if(sound.getVolume()) display.drawChar(LCDWIDTH-5-FONTWIDTH,LCDHEIGHT-17,'\24', 1);
			else display.drawChar(LCDWIDTH-5-FONTWIDTH,LCDHEIGHT-17,'x', 1);
			
			if(buttons.pressed(BTN_A) || ((frameCount>=startMenuTimer)&&(startMenuTimer != 255))){
				sound.stop(0);
				break;
			}
			
			if(buttons.pressed(BTN_C))
				changeGame();
			}
		}
	}
	display.persistence = false;
	battery.show = true;
}

void Gamebuino::begin(const __FlashStringHelper* name){
	begin(name, 0);
}

void Gamebuino::begin(){
	begin(F(""));
}

boolean Gamebuino::update() {
    if (((nextFrameMillis - millis()) > timePerFrame) && frameEndMicros) { //if time to render a new frame is reached and the frame end has ran once
        nextFrameMillis = millis() + timePerFrame;
        frameCount++;

        frameEndMicros = 0;
        frameStartMicros = micros();

        backlight.update();
        buttons.update();
        battery.update();
		sound.update();

        return true;

    } else {
        if (!frameEndMicros) { //runs once at the end of the frame
            updatePopup();
			displayBattery();
            display.update(); //send the buffer to the screen
			if(!display.persistence)
				display.clear(); //clear the buffer

            frameEndMicros = micros(); //measure the frame's end time
            frameDurationMicros = frameEndMicros - frameStartMicros;

            //            display.setTextColor(BLACK);
            //            display.setCursor(0, 40);
            //            display.print(frameDurationMicros / timePerFrame);
            //            display.print(" ");
            //            display.print(2048 - freeRam());

            //            display.setCursor(0, 32);
            //            display.print("CPU:");
            //            display.print(frameDurationMicros / timePerFrame);
            //            display.println("/1000");
            //            display.print("RAM:");
            //            display.print(2048 - freeRam());
            //            display.println("/2048");
        }
        return false;
    }
}

void Gamebuino::setFrameRate(uint8_t fps) {
    timePerFrame = 1000 / fps;
	sound.prescaler = fps / 20;
}

uint8_t Gamebuino::getCpuLoad(){
	return(frameDurationMicros/(10*timePerFrame));
}

uint16_t Gamebuino::freeRam() {
    //from http://www.controllerprojects.com/2011/05/23/determining-sram-usage-on-arduino/
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int8_t Gamebuino::menu(char** items, uint8_t length) {
#if (ENABLE_GUI > 0)
	display.persistence = false;
    int8_t activeItem = 0;
    int8_t currentY = LCDHEIGHT;
    int8_t targetY = 0;
    boolean exit = false;
    int8_t answer = -1;
    while (1) {
        if (update()) {
            if (buttons.pressed(BTN_A) || buttons.pressed(BTN_B) || buttons.pressed(BTN_C)) {
                exit = true; //time to exit menu !
                targetY = -FONTHEIGHT * length - 2; //send the menu out of the screen
                if (buttons.pressed(BTN_A)) {
                    answer = activeItem;
                    sound.playOK();
                } else {
                    sound.playCancel();
                }
            }
            if (exit == false) {
                if (buttons.repeat(BTN_DOWN,4)) {
                    activeItem++;
                    sound.playTick();
                }
                if (buttons.repeat(BTN_UP,4)) {
                    activeItem--;
                    sound.playTick();
                }
                //don't go out of the menu
                if (activeItem == length) activeItem = 0;
                if (activeItem < 0) activeItem = length - 1;

                targetY = -FONTHEIGHT * activeItem + (FONTHEIGHT+4); //center the menu on the active item
            } else { //exit :
                if ((currentY - targetY) <= 1)
                    return (answer);
            }
            //draw a fancy menu
            currentY = (currentY + targetY) / 2;
            display.setCursor(0, currentY);
            display.setTextSize(1);
            display.setTextWrap(false);
            for (byte i = 0; i < length; i++) {
                if (i == activeItem)
                    display.setCursor(3, currentY + FONTHEIGHT * activeItem);
                display.println(items[i]);
            }

            //display.fillRect(0, currentY + 3 + 8 * activeItem, 2, 2, BLACK);
			display.setColor(WHITE);
            display.drawFastHLine(0, currentY + FONTHEIGHT * activeItem - 1, LCDWIDTH);
			display.setColor(BLACK);
            display.drawRoundRect(0, currentY + FONTHEIGHT * activeItem - 2, LCDWIDTH, (FONTHEIGHT+3), 3);
        }
    }
#else
return 0;
#endif
}

void Gamebuino::keyboard(char* text, uint8_t length) {
#if (ENABLE_GUI > 0)
	display.persistence = false;
    //memset(text, 0, length); //clear the text
	text[length-1] = '\0';
    //active character in the typing area
    int8_t activeChar = 0;
    //selected char on the keyboard
    int8_t activeX = 0;
    int8_t activeY = 2;
    //position of the keyboard on the screen
    int8_t currentX = LCDWIDTH;
    int8_t currentY = LCDHEIGHT;
    int8_t targetX = 0;
    int8_t targetY = 0;

    while (1) {
        if (update()) {
            //move the character selector
            if (buttons.repeat(BTN_DOWN, 4)) {
                activeY++;
                sound.playTick();
            }
            if (buttons.repeat(BTN_UP, 4)) {
                activeY--;
                sound.playTick();
            }
            if (buttons.repeat(BTN_RIGHT, 4)) {
                activeX++;
                sound.playTick();
            }
            if (buttons.repeat(BTN_LEFT, 4)) {
                activeX--;
                sound.playTick();
            }
            //don't go out of the keyboard
            if (activeX == KEYBOARD_W) activeX = 0;
            if (activeX < 0) activeX = KEYBOARD_W - 1;
            if (activeY == KEYBOARD_H) activeY = 0;
            if (activeY < 0) activeY = KEYBOARD_H - 1;
            //set the keyboard position on screen
            targetX = -(FONTWIDTH+1) * activeX + LCDWIDTH / 2 - 3;
            targetY = -(FONTHEIGHT+1) * activeY + LCDHEIGHT / 2 - 4 - FONTHEIGHT;
            //smooth the keyboard displacement
            currentX = (targetX + currentX) / 2;
            currentY = (targetY + currentY) / 2;
            //type character
            if (buttons.pressed(BTN_A)) {
                if (activeChar < (length-1)) {
					byte thisChar = activeX + KEYBOARD_W * activeY;
					if((thisChar == 0)||(thisChar == 10)||(thisChar == 13)) //avoid line feed and carriage return
						continue;
                    text[activeChar] = thisChar;
                    text[activeChar+1] = '\0';
                }
                activeChar++;
                sound.playOK();
                if (activeChar > length)
                    activeChar = length;
            }
            //erase character
            if (buttons.pressed(BTN_B)) {
                activeChar--;
                sound.playCancel();
                if (activeChar >= 0)
                    text[activeChar] = 0;
                else
                    activeChar = 0;
            }
            //leave menu
            if (buttons.pressed(BTN_C)) {
			sound.playOK();
			    while (1) {
					if (update()) {
						display.setCursor(0,0);
						display.println(F("You entered\n"));
							display.print(text);
						display.println(F("\n\n\n\x15:okay \x16:edit"));
						if(buttons.pressed(BTN_A)){
							sound.playOK();
							return;
						}
						if(buttons.pressed(BTN_B)){
							sound.playCancel();
							break;
						}
					}
				}
            }
            //draw the keyboard
            for (int8_t y = 0; y < KEYBOARD_H; y++) {
                for (int8_t x = 0; x < KEYBOARD_W; x++) {
                    display.drawChar(currentX + x * (FONTWIDTH+1), currentY + y * (FONTHEIGHT+1), x + y * KEYBOARD_W, 1);
                }
            }
			//draw instruction
			display.setCursor(currentX-FONTWIDTH*6-2, currentY+1*(FONTHEIGHT+1));
			display.print(F("\25type"));
			display.setCursor(currentX-FONTWIDTH*6-2, currentY+2*(FONTHEIGHT+1));
			display.print(F("\26back"));
			display.setCursor(currentX-FONTWIDTH*6-2, currentY+3*(FONTHEIGHT+1));
			display.print(F("\27save"));
            //erase some pixels around the selected character
			display.setColor(WHITE);
            display.drawFastHLine(currentX + activeX * (FONTWIDTH+1) - 1, currentY + activeY * (FONTHEIGHT+1) - 2, 7);
            //draw the selection rectangle
			display.setColor(BLACK);
            display.drawRoundRect(currentX + activeX * (FONTWIDTH+1) - 2, currentY + activeY * (FONTHEIGHT+1) - 3, (FONTWIDTH+2)+(FONTWIDTH-1)%2, (FONTHEIGHT+5), 3);
            //draw keyboard outline
            //display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * (FONTWIDTH+1) + 12, KEYBOARD_H * (FONTHEIGHT+1) + 12, 8, BLACK);
            //text field
            display.drawFastHLine(0, LCDHEIGHT-FONTHEIGHT-2, LCDWIDTH);
			display.setColor(WHITE);
            display.fillRect(0, LCDHEIGHT-FONTHEIGHT-1, LCDWIDTH, FONTHEIGHT+1);
            //typed text
            display.setCursor(0, LCDHEIGHT-FONTHEIGHT);
			display.setColor(BLACK);
            display.print(text);
            //blinking cursor
            if (((frameCount % 8) < 4) && (activeChar < (length-1)))
                display.drawChar(FONTWIDTH * activeChar, LCDHEIGHT-FONTHEIGHT, '_',1);
        }
    }
#endif
}

void Gamebuino::popup(const __FlashStringHelper* text, uint8_t duration){
#if (ENABLE_GUI > 0)
    popupText = text;
    popupTimeLeft = duration;
#endif
}

void Gamebuino::updatePopup(){
#if (ENABLE_GUI > 0)
    if (popupTimeLeft){
        uint8_t yOffset = 0;
        if(popupTimeLeft<12){
            yOffset = 12-popupTimeLeft;
        }
        display.setTextSize(1);
		display.setColor(WHITE);
        display.fillRoundRect(0,LCDHEIGHT-FONTHEIGHT+yOffset-3,84,FONTHEIGHT+3,3);
		display.setColor(BLACK);
        display.drawRoundRect(0,LCDHEIGHT-FONTHEIGHT+yOffset-3,84,FONTHEIGHT+3,3);
        display.setCursor(4, LCDHEIGHT-FONTHEIGHT+yOffset-1);
        display.print(popupText);
        popupTimeLeft--;
    }
#endif
}

void Gamebuino::adjustVolume(){
#if (ENABLE_GUI > 0) || (NUM_CHANNELS > 0)
  while(1){
    if(update()==true){
      byte volume = sound.getVolume();
      display.setTextSize(1);
      display.setColor(BLACK);
      display.setCursor(24, 16);
      display.println(F("VOLUME"));
      display.drawRoundRect(24,28,36,7,3);
      if(volume)
        display.fillRoundRect(24,28,12*volume,7,3);
      if(buttons.pressed(BTN_RIGHT) || buttons.pressed(BTN_UP)){
        sound.setVolume(volume + 1);
        sound.playOK();
      }
      if(buttons.pressed(BTN_LEFT) || buttons.pressed(BTN_DOWN)){
        sound.setVolume(volume - 1);
        sound.playCancel();
      }
      if(buttons.pressed(BTN_C)){
        break;
      }
    }
  }
#endif
}

void Gamebuino::displayBattery(){
#if (ENABLE_BATTERY > 0)
    display.setColor(BLACK, WHITE);
	display.setCursor(LCDWIDTH-FONTWIDTH+1,0);
	switch(battery.level){
		case 0://battery critic, power down
			sound.stop();
			backlight.set(0);
			display.clear();
			display.print(F("NO BATTERY\n\nPLEASE\nTURN OFF"));
			display.update();
			while(1);
            /*set_sleep_mode(SLEEP_MODE_PWR_DOWN);
            sleep_enable();
			sleep_mode();
			sleep_disable();*/
			break;
		case 1: //empty battery
			if((frameCount % 16) < 8) display.print('\7'); //blinking battery
			else display.print('x');
			break;
		case 2://low battery
		case 3://full battery
		case 4://full battery
			if(battery.show){
				display.print(char(5+battery.level));
			}
			break;
	}
#endif
}

void Gamebuino::changeGame(){
	display.clear();
	display.print(F("Loading..."));
	display.update();
	//SPSR &= ~(1 << SPI2X); //clear SPI speed x2 for compatibility issues
	load_game("LOADER");
	display.persistence = false;
	display.println(F("\nNo SD card or\nno LOADER.HEX\n\n\25:Exit"));
	display.update();
	while(1){
		buttons.update();
		if(buttons.pressed(BTN_A))
			break;
		delay(50);
	}
}

boolean Gamebuino::settingsAvailable(){
	if(pgm_read_word(SETTINGS_PAGE) == SETTINGS_TOKEN)
		return true;
	else
		return false;
}

void Gamebuino::readSettings(){
	if(settingsAvailable()){
		display.contrast = pgm_read_byte(SETTINGS_PAGE+OFFSET_CONTRAST);
		backlight.backlightMin = pgm_read_byte(SETTINGS_PAGE+OFFSET_BACKLIGHT_MIN);
		backlight.backlightMax = pgm_read_byte(SETTINGS_PAGE+OFFSET_BACKLIGHT_MAX);
		backlight.ambientLightMin = pgm_read_word(SETTINGS_PAGE+OFFSET_LIGHT_MIN);
		backlight.ambientLightMax = pgm_read_word(SETTINGS_PAGE+OFFSET_LIGHT_MAX);
		
		sound.volumeMax = pgm_read_byte(SETTINGS_PAGE+OFFSET_VOLUME_MAX);
		sound.globalVolume = pgm_read_byte(SETTINGS_PAGE+OFFSET_VOLUME_DEFAULT);

		startMenuTimer = pgm_read_byte(SETTINGS_PAGE+OFFSET_START_MENU_TIMER);
		
		battery.thresolds[0] = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_CRITIC);
		battery.thresolds[1] = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_LOW);
		battery.thresolds[2] = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_MED);
		battery.thresolds[3] = pgm_read_word(SETTINGS_PAGE+OFFSET_BATTERY_FULL);
	}
	else{
		display.contrast = SCR_CONTRAST;
		backlight.backlightMin = BACKLIGHT_MIN;
		backlight.backlightMax = BACKLIGHT_MAX;
		backlight.ambientLightMin = AMBIENTLIGHT_MIN;
		backlight.ambientLightMax = AMBIENTLIGHT_MAX;
		
		sound.volumeMax = VOLUME_GLOBAL_MAX;
		sound.globalVolume = VOLUME_GLOBAL_MAX;
		
		startMenuTimer = START_MENU_TIMER;
		
		battery.thresolds[0] = BAT_LVL_CRITIC;
		battery.thresolds[1] = BAT_LVL_LOW;
		battery.thresolds[2] = BAT_LVL_MED;
		battery.thresolds[3] = BAT_LVL_FULL;
	}
}

void Gamebuino::getDefaultName(char* string){
	if(settingsAvailable()){	
		for(byte i=0; i<USERNAME_LENGTH; i++){
			string[i] = (char)pgm_read_byte(SETTINGS_PAGE+OFFSET_USERNAME+i);
		}
	}
}

boolean Gamebuino::collidePointRect(int16_t x1, int16_t y1 ,int16_t x2 ,int16_t y2, int16_t w, int16_t h){
  if((x1>=x2)&&(x1<x2+w))
    if((y1>=y2)&&(y1<y2+h))
	  return true;
  return false;
}

boolean Gamebuino::collideRectRect(int16_t x1, int16_t y1, int16_t w1, int16_t h1 ,int16_t x2 ,int16_t y2, int16_t w2, int16_t h2){
  if(collidePointRect(x1, y1, x2, y2, w2, h2))
    return true;
  if(collidePointRect(x1+w1-1, y1, x2, y2, w2, h2))
    return true;
  if(collidePointRect(x1, y1+h1-1, x2, y2, w2, h2))
    return true;
  if(collidePointRect(x1+w1-1, y1+h1-1, x2, y2, w2, h2))
    return true;
  return false;
}