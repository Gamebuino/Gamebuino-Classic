import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class bitmapEncoder extends PApplet {

public void setup() {
  size(400, 200);
  background(128);
  selectInput("Select a picture to encode", "loadFile");
}

public void draw() {
}

public void loadFile(File selection) {
  String inputPath = selection.getAbsolutePath();
  inputPath = inputPath.replace("\\", "/");
  
  int i = inputPath.lastIndexOf('.');
  String extension = inputPath.substring(i+1);
  String outputPath = inputPath.replace(extension, "txt");
  
  println(inputPath);
  encodeFile(inputPath, outputPath);
}

public void encodeFile(String inputPath, String outputPath) {

  PImage img;
  img = loadImage(inputPath);

  if (img == null) {
    text("Can't load the selected picture", 5, 15);
    return;
  }

  text("Input:", 5, 35);
  image(img, 0, 40);
  
  if ((img.width > 88) || (img.height > 48)) {
    text("Picture too large, max 88*48px", 5, 15);
    return;
  }  
  
  if ((img.width % 8 > 0) || (img.width / 8 < 1)) {
    text("Picture's width must be multiple of 8", 5, 15);
    return;
  }
  
  PrintWriter output;
  output = createWriter(outputPath);

  output.println("const byte bitmapName[] PROGMEM =");
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
        int thisColor = img.get(x+b, y);
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
  text("Output:", 205, 35);
  image(img, 200, 40);

  output.print("};");
  output.flush();
  text("Saved as " + outputPath, 5, 15);
  output.close();
}

  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "bitmapEncoder" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
