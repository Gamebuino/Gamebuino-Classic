///////////////////////////////////// UPDATE GAME
void updateGame(){
  if(!paused){
    //move the ball
    ball_x = ball_x + ball_vx;
    ball_y = ball_y + ball_vy;

    //check for ball collisions
    //collision with the top border
    if(ball_y < 0){
      ball_vy = -ball_vy;
      gb.sound.playTick();
    }
    //collision with the bottom border
    if((ball_y + ball_size) > LCDHEIGHT){
      ball_vy = -ball_vy;
      gb.sound.playTick();
    }
    //collision with the player
    if(gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, player_x, player_y, player_w, player_h)){
      ball_x = player_x + player_w;
      ball_vx = -ball_vx;
      gb.sound.playTick();
    }
    //collision with the oponent
    if(gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, oponent_x, oponent_y, oponent_w, oponent_h)){
      ball_x = oponent_x - ball_size;
      ball_vx = -ball_vx;
      gb.sound.playTick();
    }
    //collision with the left side
    if(ball_x < 0){
      oponent_score = oponent_score + 1;
      gb.sound.playCancel();
      ball_x = (LCDWIDTH-ball_size)/2;
      ball_vx = 6*random(0,2)-3; //pick -3 or 3
      ball_y = random(0,LCDHEIGHT-ball_size);
    }
    //collision with the right side
    if((ball_x + ball_size) > LCDWIDTH){
      player_score = player_score + 1;
      gb.sound.playCancel();
      ball_x = (LCDWIDTH-ball_size)/2;
      ball_vx = 6*random(0,2)-3; //pick -3 or 3
      ball_y = random(0,LCDHEIGHT-ball_size);
    }
    
    if((player_score > 9)||(oponent_score > 9)){
      player_score = 0;
      oponent_score = 0;
    }
  }
}


