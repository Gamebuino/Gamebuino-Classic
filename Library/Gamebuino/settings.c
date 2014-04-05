#ifndef SETTINGS_C
#define	SETTINGS_C

//SETTINGS YOU CAN EDIT (to save RAM and/or ROM)

#define SCR_CONTRAST 60
#define NUM_CHANNELS 1 //number of sound channels, between 0 and 4
#define ENABLE_GUI 1 //enable menu, keyboard, pop-up, volume adjust functions
#define ENABLE_BITMAPS 1 //will replace bitmaps with rectangles if disabled
#define ENABLE_BATTERY 1 //disable battery monitoring
#define ENABLE_BACKLIGHT 1 //disable automatic back-light
#define TINY_FONT 1 //0 = 5x7 font ; 1 = 3x5 font7
#define DISPLAY_ROT NOROT //set to NOROT, ROTCCW, ROT180 or ROTCW

//IT'S STRONGLY ADVISED TO LEAVE THE FOLLOWING SETTINGS ALONE

//GUI
#define KEYBOARD_W 16
#define KEYBOARD_H 8

//screen
#define SCR_CLK 13
#define SCR_DIN 11
#define SCR_DC  A2
#define SCR_CS  A1
#define SCR_RST A0

//sound
#define VOLUME_GLOBAL_MAX 3
#define VOLUME_CHANNEL_MAX 255/NUM_CHANNELS/VOLUME_GLOBAL_MAX/7 //7 is the volume max of a note

//battery voltage monitor
#define BAT_PIN A6
#define NUM_LVL 3
#define BAT_LVL_LOW		3500
#define BAT_LVL_MED		3600
#define BAT_LVL_HIGH	3700

//SD card
#define SD_CS 10

//screens back light
#define BACKLIGHT_PIN 5
//auto back-light levels
#define BACKLIGHT_MIN 0
#define BACKLIGHT_MAX 255

//ambient light sensor
#define AMBIENTLIGHT_PIN A7
//auto back-light levels
#define AMBIENTLIGHT_MIN 800 //800
#define AMBIENTLIGHT_MAX 1000 //1000
#define AMBIENTLIGHT_SMOOTHING 16

//number of buttons
#define NUM_BTN         7
//buttons ID
#if DISPLAY_ROT == 0
	#define BTN_UP      1
	#define BTN_RIGHT   2
	#define BTN_DOWN    3
	#define BTN_LEFT    0
#elif DISPLAY_ROT == ROTCCW
	#define BTN_UP      2
	#define BTN_RIGHT   3
	#define BTN_DOWN    0
	#define BTN_LEFT    1
#elif DISPLAY_ROT == ROT180
	#define BTN_UP      3
	#define BTN_RIGHT   0
	#define BTN_DOWN    1
	#define BTN_LEFT    2
#elif DISPLAY_ROT == ROTCW
	#define BTN_UP      0
	#define BTN_RIGHT   1
	#define BTN_DOWN    2
	#define BTN_LEFT    3
#endif
#define BTN_A           4
#define BTN_B           5
#define BTN_C           6
//buttons pins
#define BTN_UP_PIN      9
#define BTN_RIGHT_PIN   7
#define BTN_DOWN_PIN    6
#define BTN_LEFT_PIN    8
#define BTN_A_PIN       4
#define BTN_B_PIN       2
#define BTN_C_PIN       A3

#endif /* SETTINGS_C */