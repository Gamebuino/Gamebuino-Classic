#2015-01-028
* faster drawBitmap function by Myndale, see http://gamebuino.com/forum/viewtopic.php?f=12&t=2949&start=40#p8102
* bitmap offset when using horizontal flip fixed
* bitmap and color examples added

#2015-01-24
* minor graphic optimizations
* GRAY color added, using alternating checker pattern suggested by Drakker, see http://gamebuino.com/forum/viewtopic.php?f=12&t=2058
* SETTINGS.HEX updated for better contrast adjustment
* gb.display.frameCount added for GRAY displaying (independant from gb.frameCount)

#2015-01-11
* drawBitmap optimization by Jamish
* Minor fixes

#2015-01-09
* Circle collision example added
* Adafruit license added to the display part of the library
* Display buffer size increased to 512B so it can be used as a buffer for the SD card (or whatever)

#2014-09-07
* INVERT color added
* collideBitmapBitmap fixes
* collide examples use the new INVERT color

#2014-09-06
* Displays the battery voltage when turning off because of low battery

#2014-09-05
* collideBitmapBitmap() added
* collideRectRect() only returns true when there there it's overlapping (not just when it's only touching)
* collidePointRect(), collideRectRect() and collideBitmapBitmap() examples added
* getBitmapPixel() added

#2014-08-29
* compilation error when NUM_CHANNEL is set to 0 fixed
* crabator, ufo-race and 3D demo hex files added
* gb.display.getBuffer() added

#2014-08-07
* License LGPL 3 added

#2014-08-14
* hardware folder added for easier installation. You only have to copy and paste it to you sketch folder, you no longer have to manually install the bootloader and edit boards.txt
* SETTINGS.HEX updated to reflect the change of the last update
* bootloader updated to the experimental bootloader v2.0 by Jonnection. gamebuino_boot_only.hex only includes the bootlaoder and gamebuino_boot.hex (default one) includes SETTINGS.HEX
* all utilities removed to be moved to different repos (font editor, emulator)
* .HEX files of all the games removed

#2014-08-11
* drawPixel and getPixel are now inline for an overall 2.9x speed-up of graphic functions (suggestion by Myndale)
* SETTINGS_PAGE moved from 0x7000-128 to 0x7800-128 to avoid overwritting settings when flashing too large games. You will have to set your settings again using SETTINGS.HEX an to re-compile your games for them to be able to read the new settings.
* gb.sound.prescaler changes the sound playback speed (1=normal speed, 2=two times slower, etc.). Automatically adjusted when you use gb.setFrameRate() for a constant playback speed.

#2014-08-09
* settings.hex values wrap around fixed
* improved pickRandomSeed (even more random!)
* wrap(i, imax) macro added to core library
* .HEX files updated
  * Sokoban no longer overwrites settings
  * 3D demo updated to latest version (works on every screen now)
  * Invaders, Minsweeper, Asterocks, Stack challenge added

# 2014-08-03
* toolkit's bitmap encoder bug fixes (hex encoding, size bytes missing in array)
* sound library bug fixes
* physics and tile map examples added
* font size is set back to 1 when titleScreen() is called
* .HEX files updated and added

# 2014-07-24
* toolkit .jar added
* LOADER.HEX now shows the logo for 2 seconds before asking for saving EEPROM
* improvements and bug fixes in the sound library
* music example visualisation added
* petit_fatfs library added

# 2014-07-04
* "chains" renamed to "tracks"
* font3x3 added
* misc improvements here and there
* "community" category added in examples

# 2014-06-26
* new layer of abstraction in the sound library
  * "chains" are a succession of patterns + transpositions to apply to patterns
  * "tracks" are now named "patterns"
  * excel tracker updated to new sound system
* titleScreen simplified
  *
# 2014-06-24
* useless setters replaced by public variables:
  * gb.display.setCursor(x,y) is now gb.display.cursorX and gb.display.cursorY
  * gb.display.setTextWrap(wrap) is now gb.display.textWrap
  * gb.display.setTextSize(scale) is now gb.display.fontSize

# 2014-06-23
* back to work!
* gb.startMenu() changed to gb.titleScreen()
  * startMenuTimer now only skip it the first time it's called
* minor bitmap speed improvement
* font can now be dynamically changed using setFont();
* font example added
* font 3x5 number's changed to look more "retro"
* sound library changes (both in gamebuino lib, excel tracker and processing player):
  * note ID 63 is silent. It is named "Silence" in the excel tracker sound.changeInstruments(const uint16_t* const* instruments, uint8_t channel) added, it changes the set of instruments of a track.
  * The default instrument set is 0:square wave 1:noise
* Excel tracker
  * transpose column added
  * you now type note's name instead of ID (find their names in the "notes" tab)
  * use "Silence" instead of a note name to mute it
* bugs solved in the Processing sound player

# 2014-06-13
* a few more "const" added and "prog_char" changed to "const char". Now actually compiles with avr-gcc > 4.6.1

# 2014-06-12
* added "const" to the "font" variable for the library to compile with the latest version of avr-gcc but you can no longer switch between different font sizes because of conflicting var name
* default contrast changed from 50 to 60
* default backlight changed from 128 to 255 (new batch of Gamebuinos has dimmer LEDs)
* bootloader updated with the last version of settings.hex
* crabator show battery in menu fixed
* crabator: added backlight effects when hit

# 2014-06-10
* zero initilizers removed (unecessary use of flash memory)
* loader 
  * can't load non-hex files
  * displays a "x" in front of non-hex files
  *displays the files' size
  * number of page calculation fixed
* settings
  * sub menus fixed
  * added 'save' in every sub menu
* start menu
  * blinking cursor in the start menu slightly changed
  * mute when "B" is held during startup (it was making a small sound during 1 frame)
  * toggle volume when "B" is pressed in the startup menu
  * speaker icon placement improved, adapts depending on the font size
* sound.setVolume now applies a modulo instead of a min-max constrain
* updated HEX and ELF files

# 2014-06-09
* changelog created! \o/
* "const" added to all PROGMEM variables for compatibity with the last avr-cc update
  * It will require you to add "const" before the variables which use PROGMEM or you'll get compilation errors
* gb.titleScreen() function added. It's used to display the main menu instead of gb.begin()
  * It should be called just after gb.begin() with the arguments you were used to put in the gb.begin() function.
  * It allows you to display the main menu again whenever you want.
* The "play" icon the the start menu is now blinking, for people who didn't know what to do
* when you call the startMenu without the F("name") argent, it leaves more space for the logo
* logo of Loader, Settings, and Crabator added
* Emulator added in /utilities along with .elf files
* bitmap encoder now ask you which file you want to encode and saves the output in the same folder in a txt file
* menu() function and examples changed to actually use flash memory instead of RAM