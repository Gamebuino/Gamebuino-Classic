///////////////////////////////////// SPAWN CRATE
void spawnCrate(){
  boolean okay = false;
  while (okay == false){
    //pick a random location
    crate_x = random(0, WORLD_W) * 8;
    crate_y = random(0, WORLD_H) * 8;
    okay = true;
    //is that in a wall ?
    if(collideWorld(crate_x, crate_y, 8, 8)){
      okay = false;
    }
    //is that in the screen ?
    int x, y;
    if(screenCoord(crate_x, crate_y, x, y)){
      okay = false;
    }
  }
}

///////////////////////////////////// COLLIDE CRATE
void collideCrate(){
  if(gb.collideRectRect(crate_x+2, crate_y+2, 4, 4, playerX, playerY, playerW, playerH)){
    if (score <5){
      gb.popup(F("Earn $5 first"), 30);
      return;
    }
    if(currentWeapon<(NUMWEAPONS-1)){
      gb.popup(F("Upgraded !"), 30);
      gb.sound.play(power_up,0);
    }
    else{
      gb.popup(F("Refilled !"), 30);
    }
    score -= 5;
    spawnCrate();
    /*byte newWeapon;
     do{
     newWeapon = random(1,NUMWEAPONS);
     } 
     while (newWeapon == currentWeapon);
     currentWeapon = newWeapon;*/
    currentWeapon = min(NUMWEAPONS-1, currentWeapon+1); //upgrade to the next weapon
    ammo = weapon_ammo[currentWeapon];
    //gb.popup(weapon_name[currentWeapon], 30);
    //if(random(0,score/10)==0) //the higher is your score, the less life you will find in crates
    playerLife = min(playerLife+1, playerLifeMax);
    //gb.buzz(2000,40);
  }
}

///////////////////////////////////// DRAW CRATE
void drawCrate(){
  int x, y;
  if(screenCoord(crate_x, crate_y, x, y)){
    gb.display.drawBitmap(x, y, crateSprite);
  }
}







