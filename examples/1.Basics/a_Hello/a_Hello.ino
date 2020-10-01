//Imports the SPI library (needed to communicate with Gamebuino's screen)
#include <SPI.h>
//Imports the Gamebuino library
#include <Gamebuino.h>
//Creates a Gamebuino object named gb
Gamebuino gb;

// The setup routine runs once when Gamebuino starts up
void setup(){
  // Initialize the Gamebuino object
  gb.begin();
  //Display the main menu:
  gb.titleScreen(F("My first game"));
  gb.popup(F("Let's go!"), 100);
}

// The loop routine runs over and over again forever
void loop(){
  //Updates the gamebuino (the display, the sound, the auto backlight... everything)
  //Returns true when it's time to render a new frame (20 times/second)
  if(gb.update()){
    //Prints Hello World! on the screen
    gb.display.println(F("Hello World!"));
    //Declare a variable named count of type integer :
    int count;
    //Get the number of frames rendered and assign it to the "count" variable
    count = gb.frameCount;
    //Prints the variable "count"
    gb.display.println(count);
  }
}
