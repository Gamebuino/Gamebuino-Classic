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