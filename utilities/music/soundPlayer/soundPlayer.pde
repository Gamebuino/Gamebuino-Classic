import ddf.minim.*;
import ddf.minim.ugens.*;
Sound sound;

//Use the composer to make your own track and paste the output here:
int myTrack1[] = {0x8005,0x3089,0x208,0x238,0x0000};
int myTrack2[] = {0x0005,0x3089,0x7889,0x1468,0x0000};

void setup()
{
  size( 512, 200, P3D );
  frameRate(20);
  sound = new Sound();
  //sound.playTrack(myTrack, 0);
  sound.playTrack(myTrack1, 0);
  sound.playTrack(myTrack2, 1);
}

void draw()
{
  background( 0 );
  stroke( 255 );
  sound.updateTrack();
  sound.updateNote();
}

void mouseClicked(){
  //sound.playTrack(myTrack, defaultInstruments, 0);
}
