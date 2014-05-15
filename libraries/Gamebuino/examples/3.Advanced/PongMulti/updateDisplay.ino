///////////////////////////////////// UPDATE DISPLAY
void updateDisplay(){
  //gb.display.print(gb.getCpuLoad());
  //draw the score
  gb.display.setTextSize(2);
  gb.display.setCursor(15,16);
  gb.display.print(player_score);
  gb.display.setCursor(57,16);
  gb.display.print(oponent_score);
  gb.display.setTextSize(1);
  //draw the ball
  gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);
  //draw the player
  gb.display.fillRect(player_x, player_y, player_w, player_h);
  //draw the oponent
  gb.display.fillRect(oponent_x,oponent_y, oponent_w, oponent_h);
}


