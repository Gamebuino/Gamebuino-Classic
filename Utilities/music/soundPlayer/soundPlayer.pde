import ddf.minim.*;
import ddf.minim.ugens.*;
Sound sound;

//Use the composer to make your own track and paste the output here:
int myTrack[] = {
0x0005,                0xA00,0xA10,0xA20,0x0045,0xA00,0xA10,0xA20,
0x0005, 0x0001, 0x8849,0xA00,0xA10,0xA20,0x0045,0xA00,0xA10,0xA20,
0x0005, 0x8241, 0x7849,0xA00,0xA10,0xA20,0x0045,0xA00,0xA10,0xA20,
0x0};

int squareWaveInstrument[] = {0x0101, 0x03F7};
int noiseInstrument[] = {0x0101, 0x03FF};

int[][] defaultInstruments = {squareWaveInstrument,noiseInstrument};

void setup()
{
  size( 512, 200, P3D );
  frameRate(20);
  sound = new Sound();
  sound.playTrack(myTrack, defaultInstruments, 0);
}

void draw()
{
  background( 0 );
  stroke( 255 );
  sound.updateTrack();
  sound.updateNote();
}

void mouseClicked(){
  sound.playTrack(myTrack, defaultInstruments, 0);
}
