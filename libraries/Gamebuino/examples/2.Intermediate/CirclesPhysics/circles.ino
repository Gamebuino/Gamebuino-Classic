#define NUM_CIRCLES 8
Circle circles[NUM_CIRCLES];

void initCircles(){
  for(byte i=0; i < NUM_CIRCLES; i++){
    circles[i].r = random(3,6);
    circles[i].x = i*LCDWIDTH/NUM_CIRCLES;
    circles[i].y = random(0, LCDHEIGHT);
    circles[i].vx = random(1, 20)/10.;
    circles[i].vy = random(1, 20)/10.;
    if(i > NUM_CIRCLES/2){ //half of the ball won't move at the beginning
      circles[i].vx = 0;
      circles[i].vy = 0;
    }
  }
}

void updateCircles(){
  //apply physics on each circle one by one
  for(byte i=0; i < NUM_CIRCLES; i++){
    //add instant velocity to position on each cartesian didrection :
    circles[i].x += circles[i].vx;
    circles[i].y += circles[i].vy;
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

    //check collision with all the other circles
    //starting with the next one to avoid checking the same collision several times (A with B and B with A)
    //and to avoid check collision with itself.
    for(byte j=i+1; j < NUM_CIRCLES; j++){
      float dx = (circles[i].x - circles[j].x);
      float dy = (circles[i].y - circles[j].y);
      int dist = circles[i].r + circles[j].r;
      if ( (dx*dx + dy*dy) < dist*dist ){
        //draw a solid circle at the collision point : it the average between the coordinate, weighted by the other circle's radius
        int collisionX = (circles[i].x*circles[j].r + circles[j].x*circles[i].r) / (circles[i].r + circles[j].r);
        int collisionY = (circles[i].y*circles[j].r + circles[j].y*circles[i].r) / (circles[i].r + circles[j].r);
        gb.display.fillCircle(collisionX, collisionY, 2);
        gb.sound.playTick();

        float N = atan(dy/dx); //angle of the normal to te collision plane

        //Note that the tangent speed of the collisions are computed even if not used in this example.

        //CIRCLE I
        //composant of the speed normal to the collision plane of circle i
        float Vin = circles[i].vx * cos(N) + circles[i].vy * sin(N);
        //tangent composant of the speed
        float Vit = circles[i].vx * cos(N+PI/2) + circles[i].vy * sin(N+PI/2);
        //the composant of Vin on x and y
        float Vinx = Vin * cos(N);
        float Viny = Vin * sin(N);
        //the composant of Vit on x and y
        float Vitx = Vit * cos(N+PI/2);
        float Vity = Vit * sin(N+PI/2);

        //CIRCLE J
        //composant of the speed normal to the collision plane of circle j
        float Vjn = circles[j].vx * cos(N) + circles[j].vy * sin(N);
        //tangent composant of the speed
        float Vjt = circles[j].vx * cos(N+PI/2) + circles[j].vy * sin(N+PI/2);
        //the composant of Vin on x and y
        float Vjnx = Vjn * cos(N);
        float Vjny = Vjn * sin(N);
        //the composant of Vit on x and y
        float Vjtx = Vjt * cos(N+PI/2);
        float Vjty = Vjt * sin(N+PI/2);

        //CIRCLE I
        //only keep the tangent part
        circles[i].vx += - Vinx + Vjnx;
        circles[i].vy += - Viny + Vjny;
        //move the circle apart to avoid to to get stuck in the other circle
        circles[i].x -= Vinx;
        circles[i].y -= Viny;

        //CIRCLE
        //only keep the tangent part
        circles[j].vx += - Vjnx + Vinx;
        circles[j].vy += - Vjny + Viny;
        //move the circle apart to avoid to to get stuck in the other circle
        circles[j].x -= Vjnx;
        circles[j].y -= Vjny;
      }
    }
  }
}

void drawCircles(){
  for(byte i=0; i < NUM_CIRCLES; i++){
    gb.display.drawCircle(circles[i].x, circles[i].y, circles[i].r);
    gb.display.drawLine(circles[i].x, circles[i].y, circles[i].x + circles[i].vx*4, circles[i].y + circles[i].vy*4);
  }
}
