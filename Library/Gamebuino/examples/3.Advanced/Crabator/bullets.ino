///////////////////////////////////// SHOOT
void shoot(){
  if(ammo){
    if(nextShot == 0){
      for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
        if(!bullets_active[thisBullet]){ //look for the first inactive bullet
          bullets_active[thisBullet] = true; //set it to active as it's fired
          bullets_weapon[thisBullet] = currentWeapon;

          nextShot = weapon_rate[currentWeapon];
          ammo--;
          //spawn a bullet with some spreading
          char spreadMax = weapon_spread[currentWeapon];
          char spreadMin = (weapon_size[currentWeapon]%2==0) ? -spreadMax : -spreadMax-1;
          bullets_x[thisBullet] = playerX + playerW/2 + random(spreadMin,spreadMax+1) - weapon_size[currentWeapon]/2;
          bullets_y[thisBullet] = playerY + playerH/2 + random(spreadMin,spreadMax+1) - weapon_size[currentWeapon]/2;

          bullets_dir[thisBullet] = playerDir;
          blast_bullet = thisBullet;
          if(((currentWeapon == 1)||(currentWeapon==4))&&(gb.frameCount%2))
          {
          }
          else{
            gb.sound.play(weapons_sounds[currentWeapon], 0);
          }
          if(currentWeapon == 1){//with P90 cancel every two sounds to avoid continuous beep
            //if(gb.getFrameCount()%2)
            if(random()%2)
              gb.sound.play(p90_alternative_sound, 0);
          }
          //gb.buzz(weapon_soundFreq[currentWeapon], weapon_soundDuration[currentWeapon]);
          //player recoil
          byte recoil = weapon_playerRecoil[currentWeapon];
          moveXYDS(playerX, playerY, playerDir, -recoil);
          for(byte i = 0; i<recoil; i++){
            if(collideWorld(playerX, playerY, playerW, playerH))
              moveXYDS(playerX, playerY, playerDir, 1);
            else
              break;
          }
          if(currentWeapon == 4){ //MG42
            shake_magnitude = 1;
            shake_timeLeft = 2;
          }
          break;
        }
      }
    } 
    /*else {
     if(gb.buttons.pressed(BTN_A))
     gb.sound.playTick();
     }*/
  }
  else{
    currentWeapon = max(0, currentWeapon-1); //cut... no, magnum finally
    ammo = weapon_ammo[currentWeapon];
    //ammo = 9999;
    nextShot = 20;
    gb.popup(F("Out of ammo!"), 30);
  }
}

///////////////////////////////////// MOVE BULLETS
void moveBullets(){
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    if(bullets_active[thisBullet]){
      byte s = weapon_size[bullets_weapon[thisBullet]];
      moveXYDS(bullets_x[thisBullet], bullets_y[thisBullet], bullets_dir[thisBullet], weapon_speed[bullets_weapon[thisBullet]]);

      //collide world
      if(collideWorld(bullets_x[thisBullet], bullets_y[thisBullet], s, s)){
        bullets_active[thisBullet] = false;
        if(bullets_weapon[thisBullet] == 3){ //RPG
          blast_x = bullets_x[thisBullet];
          blast_y = bullets_y[thisBullet];
          blast_lifespan = 8;
          gb.sound.play(blast_sound, 0);
        }
        else{
        }
        //gb.buzz(700,10);
        continue;
      }

      for(byte thisMob=0; thisMob<activeMobs; thisMob++){ //for each mob

        if(gb.collideRectRect(bullets_x[thisBullet], bullets_y[thisBullet], s, s,
        mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob])){

          if(bullets_weapon[thisBullet] == 3){ //RPG
            blast_x = bullets_x[thisBullet];
            blast_y = bullets_y[thisBullet];
            blast_lifespan = 8;
            gb.sound.play(blast_sound, 0);
          } 
          else {
            //gb.sound.play(mob_damage_sound, 0);
            damageMob(thisMob, thisBullet);
          }
          bullets_active[thisBullet] = false;
          break;
        }
      }

    }
  }
}

///////////////////////////////////// EXPLODE
void explode(){
  if(blast_lifespan){
    blast_lifespan--;
    //gb.buzz(50+random(0,100),40);
    shake_magnitude = 4;
    shake_timeLeft = 2;
    //pick a random blast
    byte s = 10 + random (0,6);
    int x = blast_x + random(-4,4) -s/2;
    int y = blast_y + random(-4,4) -s/2;
    //damages
    for(byte thisMob=0; thisMob<activeMobs; thisMob++){
      if(gb.collideRectRect(mobs_x[thisMob], mobs_y[thisMob], mobs_size[thisMob], mobs_size[thisMob],
      x,y,s,s))
        damageMob(thisMob,blast_bullet);
    }
    //display
    int x_screen, y_screen;
    if(screenCoord(x, y, x_screen, y_screen))
      gb.display.fillRect(x_screen, y_screen, s, s);
  }
}

///////////////////////////////////// DRAW BULLETS
void drawBullets(){
  for(byte thisBullet = 0; thisBullet < NUMBULLETS; thisBullet++){
    if(bullets_active[thisBullet]){
      int x, y;
      if(screenCoord(bullets_x[thisBullet], bullets_y[thisBullet], x, y)){
        byte s = weapon_size[bullets_weapon[thisBullet]];
        if(s==1)
          gb.display.drawPixel(x, y);
        else
          gb.display.fillRect(x, y, s, s);
      }
    }
  }
}

///////////////////////////////////// DRAW AMMO OVERLAY
void drawAmmoOverlay(){
  if(ammo){
    gb.display.setCursor(0,LCDHEIGHT-FONTHEIGHT);
    gb.display.print(weapon_name[currentWeapon]);
    if(nextShot>2)
      gb.display.fillRect(-2,LCDHEIGHT-2,nextShot,2);
    if(currentWeapon > 0){ //don't display the ammo of the cut
      byte xOffset = 0;
      if (ammo < 100)
        xOffset += FONTWIDTH;
      if (ammo < 10)
        xOffset += FONTWIDTH;
      gb.display.setCursor(LCDWIDTH-3*FONTWIDTH+xOffset,LCDHEIGHT-FONTHEIGHT);
      gb.display.print(ammo);
    } 
    else {
      gb.display.setCursor(LCDWIDTH-3*FONTWIDTH,LCDHEIGHT-FONTHEIGHT);
      gb.display.print(F("inf"));
    }
  }
}

///////////////////////////////////// SET SPLASH
void setSplash(int x, int y){
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    if(!splash_active[thisSplash]){ //look for the first inactive splash
      splash_active[thisSplash] = true; //set it to active
      splash_x[thisSplash] = x;
      splash_y[thisSplash] = y;
      splash_dir[thisSplash] = random(0,5);
      break;
    }
  }
}

///////////////////////////////////// DRAW SPLASHES
void drawSplashes(){
  for(byte thisSplash = 0; thisSplash < NUMSPLASH; thisSplash++){
    if(splash_active[thisSplash]){
      int x, y;
      if(screenCoord(splash_x[thisSplash], splash_y[thisSplash], x, y)){ //if the splash is in the screen
        //draw it
        gb.display.drawBitmap(x-2, y-2, splashSprite, splash_dir[thisSplash], NOFLIP);
      }
      else{ //erase it if it is out of the screen
        splash_active[thisSplash] = false;
      }
    }
  }
}











































