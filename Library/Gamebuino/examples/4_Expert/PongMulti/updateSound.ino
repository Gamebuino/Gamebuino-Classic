///////////////////////////////////// UPDATE SOUND
void updateSound(){
  //make a sound if the ball touches the top or bottom border
  if((ball_y == 0) || (ball_y == LCDHEIGHT - ball_size)){
    gb.sound.playTick();
  }
  //make a sound if the ball is 1 pixel on the right of the player
  if(gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, player_x+1, player_y, player_w, player_h)){
    gb.sound.playTick();
  }
  //if the ball touches the player's border
  if(ball_x < 0){
    gb.sound.playCancel();
  }
}
