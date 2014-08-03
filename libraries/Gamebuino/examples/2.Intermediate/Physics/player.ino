
MovingBox player; //create the player

void initPlayer(){
  player.w = 4;
  player.h = 8;
  while(playerCollision()){ //pick a random player position until there is no collision
    player.x = random(0, LCDWIDTH - player.w);
    player.y = random(0, LCDHEIGHT - player.h);
  }
}

void updatePlayer(){
  ///// crouch
  if(gb.buttons.pressed(BTN_B)){
    player.h = 4;
    player.y += 4;
  }
  if(! gb.buttons.timeHeld(BTN_B)){ //if BTN_B is not held down
    if(player.h ==4){ //and the player is crouched
      //stand up
      player.h = 8;
      player.y -= 4; //move the player uppward to avoid being stuck in the floor
      if(playerCollision()){ //if it creates a collision to stand up, crouch again
        player.h = 4;
        player.y += 4;
      }
    }
  }
  
  ///// horizontal movement
  player.xv *= 0.9; //friction
  if(gb.buttons.repeat(BTN_RIGHT, 1)){
    player.xv += 0.2;
  }
  if(gb.buttons.repeat(BTN_LEFT,1)){
    player.xv -= 0.2;
  }
  player.x += player.xv; //move forward of 1 step
  //check for collisions on x axis
  if(playerCollision()){
    player.x -= player.xv; //move back of 1 step
    player.xv *= -0.5; //change the velocity direction
  }
  
  ///// vertical movement
  player.yv += 0.3; //gravity
  player.yv *= 0.95; //friction
  if(gb.buttons.timeHeld(BTN_A) > 0 & gb.buttons.timeHeld(BTN_A) < 5 ){ //higher jump by holding A down
    player.yv -= 1; //jump
  }
  if(gb.buttons.pressed(BTN_A)){
    gb.sound.playOK();
  }
  player.y += player.yv; //move forward of 1 step
  //check for collisions on y axis
  if(playerCollision()){
    player.y -= player.yv; //move back of 1 step
    player.yv *= -0.5; //change the velocity direction
  }
}

void drawPlayer(){
  gb.display.fillRect(player.x, player.y, player.w, player.h);
}

boolean playerCollision(){
  for(byte i=0; i<NUM_OBSTACLES; i++){
    if(gb.collideRectRect((int)player.x+1, (int)player.y+1, player.w-2, player.h-2, obstacles[i].x, obstacles[i].y, obstacles[i].w, obstacles[i].h)){
      return true;
    }
  }
  return false;
}

