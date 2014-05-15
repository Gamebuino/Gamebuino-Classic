///////////////////////////////////// SETUP SLAVE
void setupSlave(){
  Wire.begin(2);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

///////////////////////////////////// UPDATE SLAVE
void updateSlave(){
  byte timout = 0;
  paused = false;
  while(slave_updated == false){
    //wait for the master's interrupt
    delay(1);
    timout++;
    if(timout >= 20){
      gb.popup(F("No master"),15);
      disconnected = true;
      paused = true;
      slave_updated = false;
      break;
    }
  }
  slave_updated = false;
}

///////////////////////////////////// SLAVE SEND
// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
char output[SLAVE_DATA_BUFFER_LENGTH];
void requestEvent()
{

  //the slave can only answer with one "write" so you have to put all the variables in an string
  output[0] = PLAYER_Y; //identifier
  output[1] = player_y; //variable

  if(paused){
    output[0] = SLAVE_PAUSED;
    output[1] = SLAVE_PAUSED;
  }

  if(isMaster){
    output[0] = I_AM_MASTER;
    output[1] = I_AM_MASTER;
  }

  Wire.write(output);
}

///////////////////////////////////// SLAVE RECEIVE
// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany)
{
  while(Wire.available())    // slave may send less than requested
  {
    char data_in = Wire.read(); // receive byte per byte
    switch(data_in){
    case PLAYER_Y:
      oponent_y = Wire.read();
      break;
    case BALL_X:
      ball_x = Wire.read();
      ball_x = LCDWIDTH - ball_size - ball_x; //mirror the ball position;
      break;
    case BALL_Y:
      ball_y = Wire.read();
      break;
    case PLAYER_SCORE:
      oponent_score = Wire.read();
      break;
    case OPONENT_SCORE:
      player_score = Wire.read();
    default:
      break;
    }
  }
  slave_updated = true;
  disconnected = false;
}







