///////////////////////////////////// SPAWN ONE MOB
boolean spawnMob(byte thisMob){
  boolean okay = false;
  byte timout = 0;
  mobs_size[thisMob] = mob_size;
  mobs_life[thisMob] = mob_maxLife;
  if(!boss_nextSpawn){ //spawn big mobs every 20 kills starting from 15
    boss_freq = max(boss_freq - BOSSRATE, 1);
    boss_nextSpawn = boss_freq;
    mobs_size[thisMob] = boss_size;
    mobs_life[thisMob] = boss_maxLife;
    //gb.popup("Boss spawned !", 30);
  }
  while(okay == false){ //do the following until it's okay
    //check we are not stuck here because there is no space to spawn mobs
    /*if(timout==255){
     gb.display.setCursor(0,32);
     gb.display.setTextColor(BLACK, WHITE);
     gb.display.print("Timeout !");
     gb.display.update();
     delay(200);
     return false;
     }
     timout++;*/
    //pick a random location
    mobs_x[thisMob] = random(0, WORLD_W*2) * 4;
    mobs_y[thisMob] = random(0, WORLD_H*2) * 4;
    //and check if that position is okay
    okay = true;

    if(checkMobCollisions(thisMob)){
      okay = false;
      continue;
    }
    //spawn the mobs out of the player's view
    if(wrap(mobs_x[thisMob] - cameraX, WORLD_W*8) < LCDWIDTH){
      okay = false;
      continue;
    }
    if(wrap(mobs_y[thisMob] - cameraY, WORLD_H*8) < LCDHEIGHT){
      okay = false;
      continue;
    }
  }
  mobs_dir[thisMob] = random() % 4; //then pick a random direction
  return true;
}

///////////////////////////////////// SPAWN ALL MOBS
boolean spawnMobs(){
  /*gb.setCursor(0,32);
   gb.setTextColor(BLACK, WHITE);
   gb.print("Spawning ");*/
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){ //put mobs far away
    mobs_x[thisMob] = 9999;
    mobs_y[thisMob] = 9999;
  }
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){
    if(!spawnMob(thisMob)) //try to spawn a mob
        return false; //return false if an error occur
    /*gb.buzz(2000,3);
     gb.setCursor(0,40);
     gb.print(thisMob+1);
     gb.print("/");
     gb.print(NUMMOBS);
     gb.print(" ");
     gb.display();*/
  }
  return true;
}

///////////////////////////////////// MOVE MOBS
void moveMobs(){
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){ //for each mob
    int x = wrap(mobs_x[thisMob] - cameraX, WORLD_W*8);
    int y = wrap(mobs_y[thisMob] - cameraY, WORLD_H*8);
    //if the mob is close to the screen
    if( (distanceBetween(mobs_x[thisMob], playerX, WORLD_W*8) < (LCDWIDTH+32)) &&  (distanceBetween(mobs_y[thisMob], playerY, WORLD_H*8) < (LCDHEIGHT+32))){
      moveXYDS(mobs_x[thisMob], mobs_y[thisMob], mobs_dir[thisMob], 1); //go forward
      //if there is a collision, move a step backward and pick a new random direction
      if(checkMobCollisions(thisMob)){
        moveXYDS(mobs_x[thisMob], mobs_y[thisMob], mobs_dir[thisMob], -1);
        mobs_dir[thisMob] = random()%4;
        continue;
      }
      if(random(0,32)==0){ //randomly change movement direction
        mobs_dir[thisMob] = random()%4;
        continue;
      }
      if(random(0,16)==0){
        //if the distance between the plaer and the mob is larger on the X axis
        //if( ( abs(LCDWIDTH/2 - x) - abs(LCDHEIGHT/2 - y) ) > 0){
        if(random(0,2)){
          //get closer to the player on the X axis
          if((LCDWIDTH/2 - x) > 0){ //go to the left if the player is on the left
            mobs_dir[thisMob] = 3;
          }
          else{ // or go to the right if the player is on the right
            mobs_dir[thisMob] = 1;
          }
        }
        //if the distance between the plaer and the mob is larger on the Y axis
        else {
          //get closer to the player on the Y axis
          if((LCDHEIGHT/2 - y) > 0){ //go downward
            mobs_dir[thisMob] = 2;
          }
          else{ //go upward
            mobs_dir[thisMob] = 0;
          }
        }
      }
    }
  }
}

///////////////////////////////////// CHECK MOB COLLISIONS
boolean checkMobCollisions(byte thisMob){
  //check collision with the world
  if(collideWorld(mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob]))
    return true;
  //check collision with other mobs
  if(collideOtherMobs(thisMob))
    return true;
  return false;
}


///////////////////////////////////// CHECK IF A MOB COLLIDE ANOTHER ONE
boolean collideOtherMobs(byte thisMob){
  for(byte otherMob=0; otherMob<activeMobs; otherMob++){
    if(thisMob == otherMob) //don't check collision with iself >_<'
      continue;
    if(gb.collideRectRect(mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
    mobs_x[otherMob], mobs_y[otherMob], mobs_size[otherMob], mobs_size[otherMob])){
      return true;
    }
  }
  return false;
}

///////////////////////////////////// DRAW MOBS
void drawMobs(){
  for(byte thisMob=0; thisMob<activeMobs; thisMob++){
    //int x = wrap(mobs_x[thisMob] - cameraX + playerW/2, WORLD_W*8);
    //int y = wrap(mobs_y[thisMob] - cameraY + playerH/2, WORLD_H*8);
    int x, y;
    if(screenCoord(mobs_x[thisMob], mobs_y[thisMob], x, y)){
      if(mobs_size[thisMob] != boss_size)
        gb.display.drawBitmap(x-2, y-2, mobSprite, mobs_dir[thisMob], NOFLIP);
      else
        gb.display.drawBitmap(x-1, y-1, bossSprite, mobs_dir[thisMob], NOFLIP);
      //gb.fillRect(x, y, mobs_size[thisMob], mobs_size[thisMob], BLACK);
    }
  }
}

///////////////////////////////////// DAMAGE MOB
void damageMob(byte thisMob, byte thisBullet){
  mobs_life[thisMob] -= weapon_damage[bullets_weapon[thisBullet]];
  //recoil
  byte recoil = weapon_ennemyRecoil[bullets_weapon[thisBullet]];
  if(mobs_size[thisMob] == boss_size)
    recoil /= 4;
  moveXYDS(mobs_x[thisMob], mobs_y[thisMob], bullets_dir[thisBullet], recoil);
  if(checkMobCollisions(thisMob))
    moveXYDS(mobs_x[thisMob], mobs_y[thisMob], bullets_dir[thisBullet], -recoil);
  mobs_dir[thisMob] = (bullets_dir[thisBullet] + 2) % 4;
  //gb.buzz(1200,10);
  if(mobs_life[thisMob] <= 0){ //the mob dies
    score++;
    kills++;
    boss_nextSpawn--;
    if(bullets_weapon[thisBullet]!=3){ //if it's no the RPG
      gb.sound.play(mob_death_sound,0);
    }
    if(mobs_size[thisMob] == boss_size)
      score += 4;
    setSplash(mobs_x[thisMob], mobs_y[thisMob]);
    int x, y;
    if(screenCoord(mobs_x[thisMob], mobs_y[thisMob], x, y)){
      gb.display.fillRect(x-1, y-1, mobs_size[thisMob]+1, mobs_size[thisMob]+1);
    }
    //gb.buzz(1400,20);
    spawnMob(thisMob);
    if(activeMobs < NUMMOBS){ //if the max isn't reached
      if(activeMobs < (kills/MOBSRATE)+INITNUMMOBS){ //every 8 mobs killed
        activeMobs++; //add a mob
        spawnMob(activeMobs-1); //spawn the mob added
      }
    }
  }
  else { //the mob survives
  }
}

