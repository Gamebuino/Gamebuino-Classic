///////////////////////////////////// SETUP MASTER
void setupMaster(){
  Wire.begin();
}

///////////////////////////////////// UPDATE MASTER
void updateMaster(){
  //read from slave
  masterRead();
  updateGame();
  masterWrite();

}

///////////////////////////////////// MASTER READ
void masterRead(){
  if(Wire.requestFrom(2, SLAVE_DATA_BUFFER_LENGTH)){    // request 6 bytes from slave device #2
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
}

///////////////////////////////////// MASTER WRITE
void masterWrite(){
  Wire.beginTransmission(2); // transmit to device #2
  Wire.write(PLAYER_Y); //identifier
  Wire.write(player_y); //variable
  Wire.write(BALL_X);
  Wire.write(ball_x);
  Wire.write(BALL_Y);
  Wire.write(ball_y);
  Wire.write(PLAYER_SCORE);
  Wire.write(player_score);
  Wire.write(OPONENT_SCORE);
  Wire.write(oponent_score);
  Wire.endTransmission();    // stop transmitting
}




