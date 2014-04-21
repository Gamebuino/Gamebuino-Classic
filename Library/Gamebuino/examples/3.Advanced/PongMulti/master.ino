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
    disconnected = false;
    paused = false;
    while(Wire.available()){    // slave may send less than requested
      char data_in = Wire.read(); // receive byte per byte
      switch(data_in){
      case PLAYER_Y:
        oponent_y = Wire.read();
        break;
      case SLAVE_PAUSED:
        gb.popup(F("Slave paused"),15);
        paused = true;
        break;
      case I_AM_MASTER:
        gb.popup(F("1 master max"),15);
        paused = true;
        break;
      default:
        gb.popup(F("Wrong slave data"),15);
        paused = true;
        break;
      }
    }
  }
  else{
    gb.popup(F("Slave disconnected"),15);
    paused = true;
    disconnected = true;
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






