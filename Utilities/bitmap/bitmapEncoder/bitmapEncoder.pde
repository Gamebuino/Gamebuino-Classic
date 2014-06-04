void setup() {
  size(200, 200);

  PrintWriter output;
  output = createWriter("output.txt"); 

  PImage img;
  img = loadImage("input.bmp");
  image(img, 0, 0);

  output.println("static unsigned char PROGMEM bitmapName[] =");
  output.println("{");
  output.print("  ");
  output.print(img.width);
  output.print(",");
  output.print(img.height);
  output.println(", //width and height");

  img.loadPixels();

  for (int y = 0; y<img.height; y++) {
    output.print("  ");
    for (int x = 0; x<img.width; x+=8) {
      output.print("B");
      for (int b = 0; b<8; b++) {
        color thisColor = img.get(x+b, y);
        if (brightness(thisColor) > 100) {
          output.print("0");
          img.set(x+b, y, color(255));
        }
        else {
          output.print("1");
          img.set(x+b, y, color(0));
        }
      }
      output.print(", ");
    }
    output.println();
    if ((y%8)==7) {
      output.println();
    }
  }

  img.updatePixels();
  image(img, 0, 100);

  output.print("};");
  output.flush();
  output.close();
}

void draw() {
}

