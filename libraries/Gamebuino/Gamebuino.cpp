/* 
* File:   Gamebuino.cpp
* Author: Rodot
* 
* Created on October 1, 2013, 5:39 PM
*/

#include "Gamebuino.h"
const uint16_t startupSound[] PROGMEM = {0x0005,0x3089,0x208,0x238,0x7849,0x1468,0x0000};

const uint8_t gamebuinoLogo[] PROGMEM =
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

void Gamebuino::begin() {
	timePerFrame = 50;
	//nextFrameMillis = 0;
	//frameCount = 0;
	frameEndMicros = 1;
	startMenuTimer = 255;
	//read default settings from flash memory (set using settings.hex)
	readSettings();
	//init everything
	backlight.begin();
	backlight.set(BACKLIGHT_MAX);
	buttons.begin();
	buttons.update();
	battery.begin();
	display.begin(SCR_CLK, SCR_DIN, SCR_DC, SCR_CS, SCR_RST);
	sound.begin();
	
	//mute when B is held during start up
	if(buttons.pressed(BTN_B)){
		sound.setVolume(0);
	}
	else{ //play the startup sound on each channel for it to be louder
		#if(NUM_CHANNELS > 0)
			sound.playPattern(startupSound, 0);
		#endif
		#if(NUM_CHANNELS > 1)
			sound.playPattern(startupSound, 1);
		#endif
		#if(NUM_CHANNELS > 2)
			sound.playPattern(startupSound, 2);
		#endif
		#if(NUM_CHANNELS > 3)
			sound.playPattern(startupSound, 3);
		#endif
	}
}

void Gamebuino::titleScreen(const __FlashStringHelper* name){
	titleScreen(name, 0);
}

void Gamebuino::titleScreen(const uint8_t* logo){
	titleScreen(F(""), logo);
}

void Gamebuino::titleScreen(){
	titleScreen(F(""));
}

void Gamebuino::titleScreen(const __FlashStringHelper*  name, const uint8_t *logo){
	if(startMenuTimer){
		display.fontSize = 1;
		display.textWrap = false;
		display.persistence = false;
		battery.show = false;
		display.setColor(BLACK);
		while(1){
			if(update()){
				uint8_t logoOffset = pgm_read_byte(name)?display.fontHeight:0; //add an offset the logo when there is a name to display
				//draw graphics
				#if LCDWIDTH == LCDWIDTH_NOROT
				display.drawBitmap(0,0, gamebuinoLogo);
				if(logo){
					display.drawBitmap(0, 12+logoOffset, logo);
				}
				display.cursorX = 0;
				display.cursorY = 12; 
				#else
				display.drawBitmap(7,0, gamebuinoLogo);
				display.drawBitmap(-41,12,gamebuinoLogo);
				if(logo){
					display.drawBitmap(0, 24+logoOffset, logo);
				}
				display.cursorX = 0;
				display.cursorY = 24; 
				#endif
				display.print(name);
				
				//A button
				display.cursorX = LCDWIDTH - display.fontWidth*3 -1;
				display.cursorY = LCDHEIGHT - display.fontHeight*3 - 3;
				if((frameCount/16)%2)
				  display.println(F("\25 \20"));
				else
				  display.println(F("\25\20 "));
				//B button
				display.cursorX = LCDWIDTH - display.fontWidth*3 - 1;
				display.cursorY++;
				if(sound.globalVolume)
					display.println(F("\26\23\24"));
				else
					display.println(F("\26\23x"));
				//C button
				display.cursorX = LCDWIDTH - display.fontWidth*3 - 1;
				display.cursorY++;
				display.println(F("\27SD"));
				
				//toggle volume when B is pressed
				if(buttons.pressed(BTN_B)){
					sound.setVolume(sound.globalVolume+1);
					sound.playTick();
				}
				//leave the menu
				if(buttons.pressed(BTN_A) || ((frameCount>=startMenuTimer)&&(startMenuTimer != 255))){
					startMenuTimer = 255; //don't automatically skip the title screen next time it's displayed
					sound.stopPattern(0);
					sound.playOK();
					break;
				}
				//flash the loader
				if(buttons.pressed(BTN_C))
					changeGame();
			}
		}
		battery.show = true;
	}
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

		return true;

	} else {
		if (!frameEndMicros) { //runs once at the end of the frame
			sound.updateTrack();
			sound.updatePattern();
			sound.updateNote();
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

void Gamebuino::pickRandomSeed(){
	randomSeed(~battery.voltage * ~micros() * ~micros() + backlight.ambientLight + micros());
}

uint8_t Gamebuino::getCpuLoad(){
	return(frameDurationMicros/(10*timePerFrame));
}

uint16_t Gamebuino::getFreeRam() {
	//from http://www.controllerprojects.com/2011/05/23/determining-sram-usage-on-arduino/
	extern int __heap_start, *__brkval;
	int v;
	return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

int8_t Gamebuino::menu(const char* const* items, uint8_t length) {
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
				targetY = - display.fontHeight * length - 2; //send the menu out of the screen
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

				targetY = -display.fontHeight * activeItem + (display.fontHeight+4); //center the menu on the active item
			} else { //exit :
				if ((currentY - targetY) <= 1)
				return (answer);
			}
			//draw a fancy menu
			currentY = (currentY + targetY) / 2;
			display.cursorX = 0;
			display.cursorY = currentY;
			display.fontSize = 1;
			display.textWrap = false;
			for (byte i = 0; i < length; i++) {
				if (i == activeItem){
					display.cursorX = 3;
					display.cursorY = currentY + display.fontHeight * activeItem;
				}
				display.println((const __FlashStringHelper*)pgm_read_word(items+i));
			}

			//display.fillRect(0, currentY + 3 + 8 * activeItem, 2, 2, BLACK);
			display.setColor(WHITE);
			display.drawFastHLine(0, currentY + display.fontHeight * activeItem - 1, LCDWIDTH);
			display.setColor(BLACK);
			display.drawRoundRect(0, currentY + display.fontHeight * activeItem - 2, LCDWIDTH, (display.fontHeight+3), 3);
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
			targetX = -(display.fontWidth+1) * activeX + LCDWIDTH / 2 - 3;
			targetY = -(display.fontHeight+1) * activeY + LCDHEIGHT / 2 - 4 - display.fontHeight;
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
						//display.setCursor(0,0);
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
					display.drawChar(currentX + x * (display.fontWidth+1), currentY + y * (display.fontHeight+1), x + y * KEYBOARD_W, 1);
				}
			}
			//draw instruction
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+1*(display.fontHeight+1);
			display.print(F("\25type"));
			
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+2*(display.fontHeight+1);
			display.print(F("\26back"));
			
			display.cursorX = currentX-display.fontWidth*6-2;
			display.cursorY = currentY+3*(display.fontHeight+1);
			display.print(F("\27save"));
			
			//erase some pixels around the selected character
			display.setColor(WHITE);
			display.drawFastHLine(currentX + activeX * (display.fontWidth+1) - 1, currentY + activeY * (display.fontHeight+1) - 2, 7);
			//draw the selection rectangle
			display.setColor(BLACK);
			display.drawRoundRect(currentX + activeX * (display.fontWidth+1) - 2, currentY + activeY * (display.fontHeight+1) - 3, (display.fontWidth+2)+(display.fontWidth-1)%2, (display.fontHeight+5), 3);
			//draw keyboard outline
			//display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * (display.fontWidth+1) + 12, KEYBOARD_H * (display.fontHeight+1) + 12, 8, BLACK);
			//text field
			display.drawFastHLine(0, LCDHEIGHT-display.fontHeight-2, LCDWIDTH);
			display.setColor(WHITE);
			display.fillRect(0, LCDHEIGHT-display.fontHeight-1, LCDWIDTH, display.fontHeight+1);
			//typed text
			display.cursorX = 0;
			display.cursorY = LCDHEIGHT-display.fontHeight;
			display.setColor(BLACK);
			display.print(text);
			//blinking cursor
			if (((frameCount % 8) < 4) && (activeChar < (length-1)))
			display.drawChar(display.fontWidth * activeChar, LCDHEIGHT-display.fontHeight, '_',1);
		}
	}
#endif
}

void Gamebuino::popup(const __FlashStringHelper* text, uint8_t duration){
#if (ENABLE_GUI > 0)
	popupText = text;
	popupTimeLeft = duration+12;
#endif
}

void Gamebuino::updatePopup(){
#if (ENABLE_GUI > 0)
	if (popupTimeLeft){
		uint8_t yOffset = 0;
		if(popupTimeLeft<12){
			yOffset = 12-popupTimeLeft;
		}
		display.fontSize = 1;
		display.setColor(WHITE);
		display.fillRoundRect(0,LCDHEIGHT-display.fontHeight+yOffset-3,84,display.fontHeight+3,3);
		display.setColor(BLACK);
		display.drawRoundRect(0,LCDHEIGHT-display.fontHeight+yOffset-3,84,display.fontHeight+3,3);
		display.cursorX = 4;
		display.cursorY = LCDHEIGHT-display.fontHeight+yOffset-1;
		display.print(popupText);
		popupTimeLeft--;
	}
#endif
}

void Gamebuino::displayBattery(){
#if (ENABLE_BATTERY > 0)
	display.setColor(BLACK, WHITE);
	display.cursorX = LCDWIDTH-display.fontWidth+1;
	display.cursorY = 0;
	switch(battery.level){
	case 0://battery critic, power down
		sound.stopPattern();
		backlight.set(0);
		display.clear();
		display.fontSize = 1;
		display.print(F("NO BATTERY\n\nPLEASE\nTURN OFF"));
		display.update();
		set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		sleep_enable();
		sleep_mode();
		sleep_disable();
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
	default:
		if(battery.show){
			display.print('/');
		}
		break;
	}
#endif
}

void Gamebuino::changeGame(){
	display.clear();
	display.print(F("\35 Flashing\n  loader...\n\nDON'T TURN OFF!"));
	display.update();
	//SPSR &= ~(1 << SPI2X); //clear SPI speed x2 for compatibility issues
	SPI.setClockDivider(SPI_CLOCK_DIV128);
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
  return !( x2     >  x1+w1  || 
            x2+w2  <  x1     || 
            y2     >  y1+h1  ||
            y2+h2  <  y1     );
}