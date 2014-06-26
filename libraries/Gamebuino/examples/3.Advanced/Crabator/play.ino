///////////////////////////////////// INIT GAME
void initGame(){
  //lastScore = score;
  if(score > highscore[RANKMAX-1]){ //if the score is better than the worse high score
    saveHighscore();
  }
  //scoreDisplayTimeLeft = 64;
  score = 0;
  kills = 0;
  currentWeapon = 0; //magnum
  ammo = 9999;
  nextShot = 0;
  shake_timeLeft = 0;
  playerLife = playerLifeMax;
  boss_freq = BOSSFREQ;
  boss_nextSpawn = boss_freq;
  activeMobs = INITNUMMOBS; //6 initial mobs
  do{
    do{
      playerX = random(0, WORLD_W) * 8;
      playerY = random(0, WORLD_H) * 8;
    }
    while(collideWorld(playerX, playerY, playerW, playerH));
    cameraX = playerX - LCDWIDTH/2 + playerW/2;
    cameraY = playerY - LCDHEIGHT/2 + playerW/2;
  } 
  while(!spawnMobs()); //do that until mobs are spawned without error
  //spawn crate
  spawnCrate();
  //reset bullets
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    bullets_active[thisBullet] = false;
  }
  //reset splashes
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    splash_active[thisSplash] = false;
  }
  blast_lifespan = 0; //reset explosion
}

///////////////////////////////////// PLAY
void play(){
  gb.battery.show = false;
  byte i = 0;
  while(i < 10){
    if(gb.update()){
      gb.display.fontSize = 2;
      gb.display.cursorX = 6;
      gb.display.cursorY = 16;
      gb.display.print(F("LET'S GO!"));
      i++;
    }
  }
  gb.display.fontSize = 1;
  gb.popup(F("\x15:shoot \x16:run"), 60);
  
  while(true){
    if(gb.update()){
      if(gb.buttons.pressed(BTN_C)){
        gb.sound.playCancel();
        return;
        //gb.battery.show = true;
        //pause();
        //gb.battery.show = false;
      }
      boolean moved = false;
      if(gb.buttons.repeat(BTN_RIGHT, 1)){
        playerDir = 3;
        moved = true;
      } 
      else{
        if(gb.buttons.repeat(BTN_LEFT, 1)){
          playerDir = 1;
          moved = true;
        }
      }
      if(gb.buttons.repeat(BTN_DOWN, 1)){
        playerDir = 2;
        moved = true;
      }
      else{
        if(gb.buttons.repeat(BTN_UP, 1)){
          playerDir = 0;
          moved = true;
        }
      }
      if(moved){
        moveXYDS(playerX, playerY, playerDir, playerSpeed);
        if(collideWorld(playerX, playerY, playerW, playerH))
          moveXYDS(playerX, playerY, playerDir, -playerSpeed);
      }
      cameraX = playerX + playerW/2 - LCDWIDTH/2;
      cameraY = playerY + playerH/2 - LCDHEIGHT/2;
      shakeScreen();
      drawWorld(cameraX, cameraY);
      int x, y;
      screenCoord(playerX, playerY, x, y);
      gb.display.drawBitmap(x-1, y-1, playerSprite, playerDir, NOFLIP);
      byte thisSprite = 0;
      moveMobs();
      drawMobs();
      if(nextShot)
        nextShot--;
      if(gb.buttons.repeat(BTN_A, 1) && !gb.buttons.repeat(BTN_B, 1)){
        shoot();
      }
      if (gb.buttons.repeat(BTN_B, 1)){
        playerSpeed = 2;
      } 
      else {
        playerSpeed = 1;
      }
      moveBullets();
      drawBullets();
      explode();
      drawSplashes();
      collideCrate();
      drawCrate();
      //life remaining
      for(byte i=0; i<=playerLifeMax/2; i+=1){
        if((i*2)<=playerLife){
          gb.display.drawBitmap(LCDWIDTH-i*9+2, 0, fullHeart);
        }
        else{
          gb.display.setColor(WHITE);
          gb.display.drawBitmap(LCDWIDTH-i*9+2, 0, fullHeart);
          gb.display.setColor(BLACK,WHITE);
          gb.display.drawBitmap(LCDWIDTH-i*9+2, 0, emptyHeart);
        }
      }
      if(!playerLife){
        if((gb.frameCount%2)==0){
          shake_magnitude = 2;
          shake_timeLeft = 1;
        }
      } 
      else{
        if(playerLife == 1){
          shake_magnitude = 1;
          shake_timeLeft = 1;
        }
      }

      if(playerLife%2){ //odd number
        gb.display.drawBitmap(LCDWIDTH-(playerLife/2+1)*9+2, 0, halfHeart);
      }
      drawAmmoOverlay();
      displayScore();
      for(byte thisMob=0; thisMob<activeMobs; thisMob++){
        if(gb.collideRectRect(mobs_x[thisMob],mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
        playerX, playerY, playerW, playerH)){
          playerLife--;
          shake_magnitude = 2;
          shake_timeLeft = 4;
          if(mobs_size[thisMob] == boss_size){
            playerLife--;
            shake_magnitude = 3;
            shake_timeLeft = 4;
          }
          gb.sound.playPattern(player_damage_sound, 0);
          spawnMob(thisMob);
          if(playerLife < 0){ 
            byte timer=0;
            while(1){
              if(gb.update()){
                drawMobs();
                drawBullets();
                drawSplashes();
                drawCrate();
                drawAmmoOverlay();
                displayScore();
                drawWorld(cameraX, cameraY);
                gb.display.drawBitmap(x-1, y-1, playerSprite, playerDir, NOFLIP);
                gb.display.setColor(WHITE);
                gb.display.fillRect(0,0,timer*2,LCDHEIGHT);
                gb.display.fillRect(LCDWIDTH-timer*2,0,timer*2,LCDHEIGHT);
                gb.display.setColor(BLACK, WHITE);
                gb.display.cursorX = 12;
                gb.display.cursorY = 1;
                gb.display.print(F("GAME OVER!"));
                timer++;
                if(timer==((LCDWIDTH/4)+10))
                  break;
              }
            }
            while(1){
              if(gb.update()){
                if(score > highscore[RANKMAX-1]){ //if the score is better than the worse high score
                  gb.display.cursorX = 2+random(0,2);
                  gb.display.cursorY = 0+random(0,2);
                  gb.display.print(F("NEW HIGHSCORE"));
                }
                else{ 
                  gb.display.cursorX = 12;
                  gb.display.cursorY = 1;
                  gb.display.print(F("GAME OVER!"));
                }
                gb.display.cursorX = 0;
                gb.display.cursorY = 12;
                gb.display.print(F("You made $"));
                gb.display.print(score);
                gb.display.print(F("\nby killing\n"));
                gb.display.print(kills);
                gb.display.print(F(" crabs."));
                gb.display.cursorX = 0;
                gb.display.cursorY = 40;
                gb.display.print(F("\x15:accept"));
                if(gb.buttons.pressed(BTN_A)){
                  gb.sound.playOK();
                  break;
                }
              }
            }
            initGame();
            break;
          }
        }
      }

    }
  } 
}

///////////////////////////////////// DISPLAY SCORE
void displayScore(){
  gb.display.cursorX = 0;
  gb.display.cursorY = 0;
  gb.display.print('$');
  gb.display.println(score);
}

///////////////////////////////////// SHAKE SCREEN
void shakeScreen(){
  if(shake_timeLeft){
    shake_timeLeft--;
    cameraX += random(-shake_magnitude,shake_magnitude+1);
    cameraY += random(-shake_magnitude,shake_magnitude+1);
    byte backlightStep = gb.backlight.backlightMax / 4;
    gb.backlight.set(gb.backlight.backlightValue-random(0,backlightStep*shake_magnitude));
  }
}

///////////////////////////////////// LOAD HIGHSCORE
void loadHighscore(){
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      name[thisScore][i] = EEPROM.read(i + thisScore*(NAMELENGTH+2));
    }
    highscore[thisScore] = EEPROM.read(NAMELENGTH + thisScore*(NAMELENGTH+2)) & 0x00FF; //LSB
    highscore[thisScore] += (EEPROM.read(NAMELENGTH+1 + thisScore*(NAMELENGTH+2)) << 8) & 0xFF00; //MSB
    highscore[thisScore] = (highscore[thisScore]==0xFFFF) ? 0 : highscore[thisScore];
  }
}  

///////////////////////////////////// SAVE HIGHSCORE
void saveHighscore(){
  gb.getDefaultName(name[RANKMAX-1]);
  gb.display.setFont(font5x7);
  gb.keyboard(name[RANKMAX-1], NAMELENGTH+1);
  highscore[RANKMAX-1] = score;
  for(byte i=RANKMAX-1; i>0; i--){ //bubble sorting FTW
    if(highscore[i-1] < highscore[i]){
      char tempName[NAMELENGTH];
      strcpy(tempName, name[i-1]);
      strcpy(name[i-1], name[i]);
      strcpy(name[i], tempName);
      unsigned int tempScore;
      tempScore = highscore[i-1];
      highscore[i-1] = highscore[i];
      highscore[i] = tempScore;
    }
    else{
      break;
    }
  }
  for(byte thisScore = 0; thisScore < RANKMAX; thisScore++){
    for(byte i=0; i<NAMELENGTH; i++){
      EEPROM.write(i + thisScore*(NAMELENGTH+2), name[thisScore][i]);
    }
    EEPROM.write(NAMELENGTH + thisScore*(NAMELENGTH+2), highscore[thisScore] & 0x00FF); //LSB
    EEPROM.write(NAMELENGTH+1 + thisScore*(NAMELENGTH+2), (highscore[thisScore] >> 8) & 0x00FF); //MSB
  }
  displayHighScores();
}
