#include <SPI.h>
#include <Gamebuino.h>
Gamebuino gb;

//bitmap encoded using the Bitmap Encoder which can be found here : http://gamebuino.com/wiki/index.php?title=Download
const byte pointerBitmap[] PROGMEM = {16,22,0xC,0x0,0x12,0x0,0x12,0x0,0x12,0x0,0x12,0x0,0x13,0x80,0x12,0x70,0x12,0x4C,0x12,0x4A,0xD2,0x49,0xB0,0x9,0x90,0x1,0x90,0x1,0x40,0x1,0x40,0x1,0x20,0x1,0x20,0x2,0x10,0x2,0x10,0x2,0x8,0x4,0x8,0x4,0xF,0xFC,};
void setup(){
  gb.begin();
  gb.titleScreen(F("Bitmap example"));
}

void loop(){
  if(gb.update()){
    //draw the pointer bitmap at coordinates x=0 and y=0 (top left corner)
    gb.display.drawBitmap(0, 0, pointerBitmap);
    
    //draw the pointer bitmap at coordinates x=18 and y=0 with an horizontal flip
    gb.display.drawBitmap(18, 0, pointerBitmap, NOROT, FLIPH);
    
    //flip value can be 
    //NOFLIP : the bitmap is not flipped
    //FLIPH : flipped horizontally (like using a vertical mirror)
    //FLIPV : flipped vertically (like using an horizontal mirror)
    //FLIPVH : flipped both horizontally and vertically
    
    //rotation value can be :
    //NOROT : no rotation
    //ROTCW : 90° rotation clockwise
    //ROTCCW : 90° rotation counter clockwise
    //ROT180 : half turn rotation
    
    //the rotation and flipping values can be stored in bytes :
    byte rotation = NOROT;
    byte flip = NOFLIP;
    
    //then you can change the rotation and flipping depending on conditions
    //here we use the buttons to  rotate and flip the bitmap
    if(gb.buttons.repeat(BTN_A,1)){
      flip = FLIPH;
    }
    if(gb.buttons.repeat(BTN_B,1)){
      flip = FLIPV;
    }
    if(gb.buttons.repeat(BTN_RIGHT,1)){
      rotation = ROTCW;
    }
    if(gb.buttons.repeat(BTN_LEFT,1)){
      rotation = ROTCCW;
    }
    if(gb.buttons.repeat(BTN_DOWN,1)){
      rotation = ROT180;
    }
    
    //draw the pointer bitmap at coordinates x=36 and y=0 with the rotation and flip defined above
    gb.display.drawBitmap(36, 0, pointerBitmap, rotation, flip);
    
    //don't forget to allow the user to get back to the title screen when C is used :
    if(gb.buttons.pressed(BTN_C)){
      gb.titleScreen(F("Bitmap example"));
    }
  }
}
