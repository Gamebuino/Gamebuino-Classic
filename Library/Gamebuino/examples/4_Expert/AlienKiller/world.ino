///////////////////////////////////// SET WORLD
void setWorld(const uint8_t *world, uint8_t w, uint8_t h, const uint8_t *tiles){
  _world = world;
  _worldWidth = w;
  _worldHeight = h;
  _tiles = tiles;
  _byteWidth = (_worldHeight + 7) / 8;
}

///////////////////////////////////// GET TILE
boolean getTile(uint8_t i, uint8_t j){
  if(pgm_read_byte(_world + (j%_worldHeight)*_byteWidth + (i%_worldWidth)/8) & (B10000000 >> (i % 8)))
    return true;
  else
    return false;
}

///////////////////////////////////// DRAW WORLD
void drawWorld(int16_t x, int16_t y){
  const uint8_t *world = _world,
  *tiles = _tiles;
  int8_t i, j,
  w = _worldWidth,
  h = _worldHeight;
  x = wrap(x,w*8);
  y = wrap(y,h*8);
  //println(x);
  //println(y);
  for(j=y/8; j < (LCDHEIGHT/8 + y/8 + 1); j++) {
    for(i=x/8; i < (LCDWIDTH/8 + x/8 + 1); i++ ) {
      if(getTile(i, j)) {
        gb.display.drawBitmap(i*8 - x, j*8 - y, tiles);
      }
    }
  }
}

///////////////////////////////////// COLLIDE WORLD
boolean collideWorld(int16_t x, int16_t y, uint8_t w, uint8_t h){
  if(getTile(x/8, y/8))
    return true;
  if(getTile((x+w-1)/8, y/8))
    return true;
  if(getTile((x+w-1)/8, (y+h-1)/8))
    return true;
  if(getTile(x/8, (y+h-1)/8))
    return true;
  return false;
}


