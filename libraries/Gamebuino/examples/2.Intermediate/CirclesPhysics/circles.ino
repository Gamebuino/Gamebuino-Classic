#define NUM_CIRCLES 16
Circle circles[NUM_CIRCLES];

void initCircles(){
  for(byte i=0; i < NUM_CIRCLES; i++){
    circles[i].r = 2;
    circles[i].x = i*LCDWIDTH/NUM_CIRCLES;
    circles[i].y = random(0, LCDHEIGHT);
    circles[i].vx = random(1, 20)/10.;
    circles[i].vy = random(1, 20)/10.;
    if(i >= NUM_CIRCLES/2){ //half of the ball won't move at the beginning
      circles[i].vx = 0;
      circles[i].vy = 0;
    }
  }
}

void updateCircles(){
  //apply physics on each circle one by one
  for(byte i=0; i < NUM_CIRCLES; i++){
    //add instant velocity to position on each cartesian direction :
    circles[i].x += circles[i].vx;
    circles[i].y += circles[i].vy;

    //check collision with all the other circlesr
    //starting with the next one to avoid checking the same collision several times (A with B and B with A)
    //and to avoid check collision with itself.
    for(byte j=i+1; j < NUM_CIRCLES; j++){
      float dx = (circles[j].x - circles[i].x);
      float dy = (circles[j].y - circles[i].y);
      float dist = circles[i].r + circles[j].r;
      if ( (dx*dx + dy*dy) < dist*dist ){


        float N = atan(dy/dx); //angle of the normal to te collision plane
        if(dx < 0){
          N += PI;
        }

        //Note that the tangent speed of the collisions are computed even if not used in this example.

        //CIRCLE I
        //composant of the speed normal to the collision plane of circle i
        float Vin = circles[i].vx * cos(N) + circles[i].vy * sin(N);
        //the composant of Vin on x and y
        float Vinx = Vin * cos(N);
        float Viny = Vin * sin(N);

        //CIRCLE J
        //composant of the speed normal to the collision plane of circle j
        float Vjn = circles[j].vx * cos(N) + circles[j].vy * sin(N);
        //the composant of Vin on x and y
        float Vjnx = Vjn * cos(N);
        float Vjny = Vjn * sin(N);

        //CIRCLE I
        circles[i].vx += - Vinx + Vjnx;
        circles[i].vy += - Viny + Vjny;

        //CIRCLE
        circles[j].vx += - Vjnx + Vinx;
        circles[j].vy += - Vjny + Viny;

        //move the first circle to the surface of the second to avoid it getting stuck inside
        circles[i].x = circles[j].x - cos(N) * dist;
        circles[i].y = circles[j].y - sin(N) * dist;
      }
    }

    //prevent circles from going out of the screen :
    if(circles[i].x < 0){
      circles[i].vx *= -1;
      circles[i].x = 0;
    }
    if(circles[i].x > LCDWIDTH){
      circles[i].vx *= -1;
      circles[i].x = LCDWIDTH;
    }

    if(circles[i].y < 0){
      circles[i].vy *= -1;
      circles[i].y = 0;
    }
    if(circles[i].y > LCDHEIGHT){
      circles[i].vy *= -1;
      circles[i].y = LCDHEIGHT;
    }
  }
}

void drawCircles(){
  for(byte i=0; i < NUM_CIRCLES; i++){
    gb.display.fillCircle(circles[i].x, circles[i].y, circles[i].r);
  }
}
