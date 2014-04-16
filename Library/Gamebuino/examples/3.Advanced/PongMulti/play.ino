///////////////////////////////////// PLAY 
/*void play(){
  while(1){
    if(gb.update()){
      if(gb.buttons.pressed(BTN_C)){
        break;
      }
      if(!paused){
        if(gb.buttons.repeat(BTN_UP, 1)){
          player_y = max(0, player_y - player_vy);
        }
        if(gb.buttons.repeat(BTN_DOWN, 1)){
          player_y = min(LCDHEIGHT, player_y + player_vy);
        }
      }

      if(singlePlayer){
        updateGame();
      }
      else{
        if(isMaster){
          gb.display.println(" master");
          updateMaster();
        }
        else {
          gb.display.println(" slave");
          updateSlave();
        }
      }

      gb.display.print(gb.getCpuLoad());
      gb.display.fillRect(player_x, player_y, player_w, player_h, BLACK);
      gb.display.fillRect(LCDWIDTH-oponent_w,oponent_y, oponent_w, oponent_h, BLACK);
      gb.display.fillRect(ball_x, ball_y, ball_size, ball_size, BLACK);
    }
  }
}*/





