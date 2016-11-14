//imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//imports the Gamebuino library
#include <Gamebuino.h>
//creates a Gamebuino object named gb
Gamebuino gb;

// the setup routine runs once when Gamebuino starts up
void setup(){
  // initialize the Gamebuino object
  gb.begin();
  //display the main menu:
  gb.titleScreen(F("My first game"));
  gb.popup(F("Let's go!"), 100);
}

// the loop routine runs over and over again forever
void loop(){
  //updates the gamebuino (the display, the sound, the auto backlight... everything)
  //returns true when it's time to render a new frame (20 times/second)
  if(gb.update()){
    //prints Hello World! on the screen
    gb.display.println(F("Hello World!"));
    //declare a variable named count of type integer :
    int count;
    //get the number of frames rendered and assign it to the "count" variable
    count = gb.frameCount;
    //prints the variable "count"
    gb.display.println(count);
  }
}
