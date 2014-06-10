class Sound {
  Minim minim;
  AudioOutput out;
  Oscil[] waves;
  Wavetable noise;
  float frequencies[] = {
    116.5, 123.5, 130.8, 138.6, 146.8, 155.6, 164.8, 174.6, 185.0, 196.0, 207.7, 220.0, 233.1, 246.9, 261.6, 277.2, 293.7, 311.1, 329.6, 349.2, 370.0, 392.0, 415.3, 440.0, 466.2, 493.9, 523.3, 554.4, 587.3, 622.3, 659.3, 698.5, 740.0, 784.0, 830.6, 880.0, 932.3, 987.8, 1046.5, 1108.7, 1174.7, 1244.5, 1318.5, 1396.9, 1480.0, 1568.0, 1661.2, 1760.0, 1864.7, 1975.5, 2093.0, 2217.5, 2381.0, 2597.4, 2857.1, 3174.6, 3571.4, 4081.6, 4761.9, 5714.3, 7142.9, 9523.8, 14285.7, 28571.4
  };

  // track data
  int trackData[][] = new int[4][];
  int trackInstruments[][][]= new int[4][][];
  boolean trackLooping[] = {false, false, false, false};
  int trackCursor[] = {0, 0, 0, 0};
  boolean trackPlaying[] = {false, false, false, false};

  //Note data
  int notePitch[] = {0, 0, 0, 0};
  int noteDuration[] = {0, 0, 0, 0};
  int noteVolume[] = {0, 0, 0, 0};
  boolean notePlaying[] = {false, false, false, false};

  // commands data
  int commandsCounter[] = {0, 0, 0, 0};
  int volumeSlideStepDuration[] = {0, 0, 0, 0};
  int volumeSlideStepSize[] = {0, 0, 0, 0};
  int arpeggioStepDuration[] = {0, 0, 0, 0};
  int arpeggioStepSize[] = {0, 0, 0, 0};
  int tremoloStepDuration[] = {0, 0, 0, 0};
  int tremoloStepSize[] = {0, 0, 0, 0};

  //Instrument data
  int instrumentData[][] = new int[4][10];
  int instrumentLength[] = {0, 0, 0, 0}; //number of steps in the instrument
  int instrumentLooping[] = {0, 0, 0, 0}; //how many steps to loop on when the last step of the instrument is reached
  int instrumentCursor[] = {0, 0, 0, 0}; //which step is being played
  int instrumentNextChange[] = {0, 0, 0, 0}; //how many frames before the next step

  //current step data
  int stepVolume[] = {0, 0, 0, 0};
  int stepPitch[] = {0, 0, 0, 0};

  Sound() {
    minim = new Minim( this );
    out   = minim.getLineOut();
    noise = Waves.square(0.5);
    noise.normalize();
    waves = new Oscil[4];
    for (int i=0; i < waves.length; i++) {
      waves[i] = new Oscil(0, 0, Waves.SQUARE );
      waves[i].patch( out );
    }
  }

  void playTrack(int[] track, int[][] instruments, int channel) {
    println("--- new track ---");
    trackData[channel] = track;
    trackInstruments[channel] = instruments.clone();
    trackCursor[channel] = 0;
    trackPlaying[channel] = true;
    noteVolume[channel] = 9;
    //reinit commands
    volumeSlideStepDuration[channel] = 0;
    arpeggioStepDuration[channel] = 0;
    tremoloStepDuration[channel] = 0;
  }


void updateTrack(){
  //println("Updating track");
  for (int i=0; i<4; i++){
    if(trackPlaying[i]){
      if(noteDuration[i]==0){//if the end of the previous note is reached
        
        int data =  trackData[i][trackCursor[i]];
        
        if(data == 0){ //end of the track reached
          println("Track end reached");
          if(trackLooping[i] == true){
            trackCursor[i] = 0;
            data = trackData[i][trackCursor[i]];
          }
          else{
            trackPlaying[i] = false;
            stopNote(i);
            continue;
          }
        }

        while ((data & 0x0001) != 0){ //read all commands and instrument changes
          print("Cmd:    ");
          data >>= 2;
          int command = data & 0x0F;
          data >>= 4;
          switch(command){
          case 0: //volume
            println("Volume:" + (data & 0x1F));
            noteVolume[i] = data & 0x1F;
            break;
          case 1: //instrument
            print("Instr:" + data);
            instrumentData[i] = trackInstruments[i][data].clone();
            println(" Length:" + instrumentData[i].length);
            instrumentLength[i] = instrumentData[i][0] & 0x00FF; //8 LSB
            instrumentLooping[i] = instrumentData[i][0] >> 8; //8 MSB - check that the loop is shorter than the instrument length
            break;
          case 2: //volume slide
            println("Volume Slide");
            volumeSlideStepDuration[i] = data & 0x1F;
            data >>= 5;
            volumeSlideStepSize[i] = data - 16;
            break;
          case 3:
            println("Arpeggio");
            arpeggioStepDuration[i] = data & 0x1F;
            data >>= 5;
            arpeggioStepSize[i] = data - 16;
            break;
          case 4:
            println("Tremolo");
            tremoloStepDuration[i] = data & 0x1F;
            data >>= 5;
            tremoloStepSize[i] = data - 16;
            break;
          default:
            break;
          }
          trackCursor[i]++;
          data = trackData[i][trackCursor[i]];
        }
        data >>= 2;

        int pitch = data & 0x003F;
        data >>= 6;
        
        int duration = data;
        
        println("Note:   " + "Pitch:" + pitch + " Dur:" + duration);
        //PLAY NOTE
        //set note
        notePitch[i] = pitch % frequencies.length; //limit to the number of available pitches
        noteDuration[i] = duration;
        //reinit vars
        instrumentNextChange[i] = 0;
        instrumentCursor[i] = 0;
        notePlaying[i] = true;
        commandsCounter[i] = 0;
        
        trackCursor[i]++;
      }
    }
  }
}

  void stopNote(int channel) {
    println("Stopping channel");
    notePlaying[channel] = false;
    waves[0].setAmplitude(0);
  }

  void updateNote() {
    for ( int i = 0; i < out.bufferSize() - 1; i++ )
    {
      // find the x position of each buffer value
      float x1  =  map( i, 0, out.bufferSize(), 0, width );
      float x2  =  map( i+1, 0, out.bufferSize(), 0, width );
      // draw a line from one buffer position to the next for both channels
      line( x1, 50 + out.left.get(i)*50, x2, 50 + out.left.get(i+1)*50);
      line( x1, 150 + out.right.get(i)*50, x2, 150 + out.right.get(i+1)*50);
    }

    text( "Frequencies   " + waves[0].frequency.getLastValue() + "   " + waves[1].frequency.getLastValue() + "   " + waves[2].frequency.getLastValue() + "   " + waves[3].frequency.getLastValue(), 5, 15 );
    text( "Amplitudes    " + waves[0].amplitude.getLastValue() + "   " + waves[1].amplitude.getLastValue() + "   " + waves[2].amplitude.getLastValue() + "   " + waves[3].amplitude.getLastValue(), 5, 30 );

    //generate the noise
           for(int k = 0; k < noise.size(); k+=2){
              //int random = (int)random(0,2)*2-1;
              float random = random(-1,1);
              for(int j=0; j<2; j++){
                if((k+j) >= noise.size()) break;
                if(k > noise.size()/2)
                  noise.set(k+j,random);
              }
            }

    //println("Updating Note");
    for (int i = 0; i < 4; i++) {
      if (notePlaying[i]) {

        if (noteDuration[i] == 0) {
          
          stopNote(i);
          continue;
        } 
        else {
          noteDuration[i]--;
        }

        if (instrumentNextChange[i] == 0) {
          //print("instrument step:");
          //read the step data from the progmem and decode it
          int thisStep = instrumentData[i][1 + instrumentCursor[i]];
          stepVolume[i] = thisStep & 0x0007;
          thisStep >>= 3;

          int stepNoise = thisStep & 0x0001;
          thisStep >>= 1;

          int stepDuration = thisStep & 0x003F;
          thisStep >>= 6;

          stepPitch[i] = thisStep;
          
          //println("  Volume:" + stepVolume[i] + " Duration:" + stepDuration + " Pitch:" + stepPitch[i]);

          //apply the step settings
          instrumentNextChange[i] = stepDuration;

          if(stepNoise > 0){
            waves[i].setWaveform(noise);
          }
          else{
            waves[i].setWaveform(Waves.square(0.5));
          }

          instrumentCursor[i]++;

          if (instrumentCursor[i] >= instrumentLength[i]) {
            if (instrumentLooping[i] != 0) {
              instrumentCursor[i] = instrumentLength[i] - instrumentLooping[i];
            } 
            else {
              stopNote(i);
            }
          }
        }
        instrumentNextChange[i]--;

        commandsCounter[i]++;

        //println("Updating output");
        //UPDATE VALUES  
        //pitch
        int pitch = notePitch[i] + stepPitch[i];
        if(arpeggioStepDuration[i] > 0)
          pitch += commandsCounter[i] / arpeggioStepDuration[i] * arpeggioStepSize[i];
        pitch = (pitch + frequencies.length) % frequencies.length; //wrap
        sound.waves[i].setFrequency( frequencies[pitch] );
        //volume
        int volume = noteVolume[i];
        if (volumeSlideStepDuration[i] > 0)
          volume += (volumeSlideStepSize[i] * commandsCounter[i] / volumeSlideStepDuration[i]);
        if (tremoloStepDuration[i] > 0)
          volume += tremoloStepSize[i] * ((commandsCounter[i]/tremoloStepDuration[i]) % 2);
        volume = constrain(volume, 0, 9);
        volume *= stepVolume[i];
        sound.waves[i].setAmplitude((float)volume/64);
      }
    }
  }
}
