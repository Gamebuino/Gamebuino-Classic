///////////////////////////////////// UPDATE MASTER
void updateLocal(){
  moveOponent();
  updateGame();
}

//////////////////////////////////// MOVE OPONENT
void moveOponent() {
    if(!paused){
    if(gb.buttons.repeat(BTN_B, 1)){
      oponent_y = max(0, oponent_y - oponent_vy);
    }
    if(gb.buttons.repeat(BTN_A, 1)){
      oponent_y = min(LCDHEIGHT-oponent_h, oponent_y + oponent_vy);
    }
  }
}

