/* 
 * File:   Gamebuino.cpp
 * Author: Rodot
 * 
 * Created on October 1, 2013, 5:39 PM
 */

#include "Gamebuino.h"
char loader[] = "LOADER";
PROGMEM uint16_t startupSound[] = {0x1E, 0x10, 0x301E, 0x10, 0x602E, 0x602C, 0x602A, 0x6028, 0x6026, 0x6024, 0x6022, 0x80, 0x0000};

Gamebuino::Gamebuino() {
}

void Gamebuino::begin(const __FlashStringHelper*  name, const uint8_t *logo) {
    timePerFrame = 50;
    nextFrameMillis = 0;
    frameCount = 0;
	frameEndMicros = 1;

    backlight.begin();
    buttons.begin();
    battery.begin();
    sound.begin();
    display.begin(SCR_CLK, SCR_DIN, SCR_DC, SCR_CS, SCR_RST);
	
    backlight.set(BACKLIGHT_MAX);
	display.persistence = true;
	
	uint8_t offset = 0;
	if(logo){
		offset = pgm_read_byte(logo)+2; //offset by the logo width
		display.drawBitmap(0, 12, logo);
	}
	display.setCursor(offset,12); 
	display.print(name);
	if(logo){
	}
	
    sound.play(startupSound, 0);
	while(1){
		if(update()){
			if(buttons.pressed(BTN_A)){
				sound.stop(0);
				break;
			}
			if(buttons.pressed(BTN_B)){
				sound.setGlobalVolume(0);
			}
			if(!sound.getGlobalVolume()){
				display.drawChar(72,31,'x',BLACK, WHITE, 1);
			}
			if(buttons.pressed(BTN_C))
				changeGame();
			
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

        return true;

    } else {
        if (!frameEndMicros) { //runs once at the end of the frame

            sound.update();
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

uint32_t Gamebuino::getFrameCount() {
    return frameCount;
}

void Gamebuino::setFrameRate(uint8_t fps) {
    timePerFrame = 1000 / fps;
}

void Gamebuino::setTimePerFrame(uint8_t time) {
    timePerFrame = time;
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
                if (buttons.pressed(BTN_DOWN)) {
                    activeItem++;
                    sound.playTick();
                }
                if (buttons.pressed(BTN_UP)) {
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
            display.setTextColor(BLACK);
            display.setTextWrap(false);
            for (byte i = 0; i < length; i++) {
                if (i == activeItem)
                    display.setCursor(3, currentY + FONTHEIGHT * activeItem);
                display.println(items[i]);
            }

            //display.fillRect(0, currentY + 3 + 8 * activeItem, 2, 2, BLACK);
            display.drawFastHLine(0, currentY + FONTHEIGHT * activeItem - 1, LCDWIDTH, WHITE);
            display.drawRoundRect(0, currentY + FONTHEIGHT * activeItem - 2, LCDWIDTH, (FONTHEIGHT+3), 3, BLACK);
        }
    }
#else
return 0;
#endif
}

void Gamebuino::keyboard(char* text, uint8_t length) {
#if (ENABLE_GUI > 0)
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
                    display.drawChar(currentX + x * (FONTWIDTH+1), currentY + y * (FONTHEIGHT+1), x + y * KEYBOARD_W, BLACK, BLACK, 1);
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
            display.drawFastHLine(currentX + activeX * (FONTWIDTH+1) - 1, currentY + activeY * (FONTHEIGHT+1) - 2, 7, WHITE);
            /*display.drawFastHLine(currentX + activeX * FONTWIDTH - 1, currentY + activeY * FONTHEIGHT - 1, 7, WHITE);
            display.drawFastHLine(currentX + activeX * FONTWIDTH - 1, currentY + activeY * FONTHEIGHT + (FONTHEIGHT+1), 7, WHITE);*/
            //draw the selection rectangle
            display.drawRoundRect(currentX + activeX * (FONTWIDTH+1) - 2, currentY + activeY * (FONTHEIGHT+1) - 3, (FONTWIDTH+2)+(FONTWIDTH-1)%2, (FONTHEIGHT+5), 3, BLACK);
            //draw keyboard outline
            //display.drawRoundRect(currentX - 6, currentY - 6, KEYBOARD_W * (FONTWIDTH+1) + 12, KEYBOARD_H * (FONTHEIGHT+1) + 12, 8, BLACK);
            //text field
            display.drawFastHLine(0, LCDHEIGHT-FONTHEIGHT-2, LCDWIDTH, BLACK);
            display.fillRect(0, LCDHEIGHT-FONTHEIGHT-1, LCDWIDTH, FONTHEIGHT+1, WHITE);
            //typed text
            display.setCursor(0, LCDHEIGHT-FONTHEIGHT);
            display.setTextColor(BLACK, BLACK);
            display.print(text);
            //blinking cursor
            if (((frameCount % 8) < 4) && (activeChar < (length-1)))
                display.drawChar(FONTWIDTH * activeChar, LCDHEIGHT-FONTHEIGHT, '_', BLACK, BLACK, 1);
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
        display.setTextColor(BLACK, WHITE);
        display.fillRoundRect(0,LCDHEIGHT-FONTHEIGHT+yOffset-3,84,FONTHEIGHT+3,3,WHITE);
        display.drawRoundRect(0,LCDHEIGHT-FONTHEIGHT+yOffset-3,84,FONTHEIGHT+3,3,BLACK);
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
      byte volume = sound.getGlobalVolume();
      display.setTextSize(1);
      display.setTextColor(BLACK, BLACK);
      display.setCursor(24, 16);
      display.println("VOLUME");
      display.drawRoundRect(24,28,36,7,3,BLACK);
      if(volume)
        display.fillRoundRect(24,28,12*volume,7,3,BLACK);
      if(buttons.pressed(BTN_RIGHT) || buttons.pressed(BTN_UP)){
        sound.setGlobalVolume(volume + 1);
        sound.playOK();
      }
      if(buttons.pressed(BTN_LEFT) || buttons.pressed(BTN_DOWN)){
        sound.setGlobalVolume(volume - 1);
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
    display.setTextColor(BLACK, WHITE);
	display.setCursor(LCDWIDTH-FONTWIDTH+1,0);
	if(!battery.getLevel()){
		if((frameCount % 16) < 8) { //blink
			display.print(char(7));
			if(!(frameCount % 16)){
				sound.playTick();
			}
		}
		else{
		    display.print('x');
		}
	}
	else
		if(battery.show){
			display.print(char(7 + battery.getLevel() - 1));
		}
#endif
}

void Gamebuino::changeGame(){
	display.clear();
	display.print("Loading...");
	display.update();
	load_game(loader);
	display.persistence = false;
	while(1){
		if(update()){
			display.println("\nNo SD card or\nno LOADER.HEX\n\nA:Exit");
			if(buttons.pressed(BTN_A))
				break;
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