boolean slave_updated = false;

///////////////////////////////////// SETUP SLAVE
void setupSlave(){
  isMaster = false;
  Wire.begin(2);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

///////////////////////////////////// UPDATE SLAVE
void updateSlave(){
  byte timout = 0;
  while(slave_updated == false){
    //wait for the master's interrupt
    delay(1);
    timout++;
    if(timout >= 20){
      //gb.display.setCursor(4,20);
      //gb.display.print("DISCONNECTED!");
      //gb.display.update();
      gb.popup("DISCONNECTED!",15);
      paused = true;
      slave_updated = false;
      break;
    }
  }
  slave_updated = false;
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
char output[2];
void requestEvent()
{
  //the slave can only answer with one "write" so you have to put all the variables in an string
  output[0] = PLAYER_Y; //identifier
  output[1] = player_y; //variable
  Wire.write(output);
}

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
      break;
    case BALL_Y:
      ball_y = Wire.read();
      break;
    default:
      break;
    }
  }
  slave_updated = true;
  paused = false;
}

