///////////////////////////////////// SETUP MASTER
void setupMaster(){
  isMaster = true;
  Wire.begin();
}

///////////////////////////////////// UPDATE MASTER
void updateMaster(){
  //read from slave
  if(Wire.requestFrom(2, 2)){    // request 6 bytes from slave device #2
    paused = false;
    while(Wire.available()){    // slave may send less than requested
      char data_in = Wire.read(); // receive byte per byte
      switch(data_in){
      case PLAYER_Y:
        oponent_y = Wire.read();
        break;
      default:
        break;
      }
    }
  }
  else{
    gb.popup("DISCONNECTED!",15);
    paused = true;
  }

  if(!paused){
    ball_x += ball_vx;
    ball_y += ball_vy;
    if(ball_x<0)
      ball_vx=1;
    if((ball_x+ball_size)>LCDWIDTH)
      ball_vx=-1;
    if(ball_y<0)
      ball_vy=1;
    if((ball_y+ball_size)>LCDHEIGHT)
      ball_vy=-1;
  }

  //write to slave
  Wire.beginTransmission(2); // transmit to device #2
  Wire.write(PLAYER_Y); //identifier
  Wire.write(player_y); //variable
  Wire.write(BALL_X);
  Wire.write(ball_x);
  Wire.write(BALL_Y);
  Wire.write(ball_y);
  Wire.endTransmission();    // stop transmitting

}





