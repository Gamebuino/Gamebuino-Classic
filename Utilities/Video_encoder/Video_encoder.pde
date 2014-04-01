import processing.video.*;
Movie myMovie;

public void setup() {
  myMovie = new Movie(this, "D:/Downloads/Movies/_to_watch/12 Angry Men (1957)/12.Angry.Men.1957.720p.BRrip.x264.YIFY.mp4");
  myMovie.loop();
  size(400, 400);
  noSmooth();
  background(200);
}

void draw() {
  if (myMovie.available() == true) {
    myMovie.read();
    PImage img = createImage(84, 48, RGB);
    float ratio;
    int xOffset, yOffset;
    float ratiow = (float) myMovie.width / 84;
    float ratioh = (float) myMovie.height / 48;
    if (ratiow < ratioh) {
      ratio = ratiow;
      xOffset = 0;
      yOffset = (int) (48 - myMovie.height / ratio) / 2;
    } 
    else {
      ratio = ratioh;
      xOffset = (int) (84 - myMovie.width / ratio) / 2;
      yOffset = 0;
    }
    img.copy(myMovie, 0, 0, myMovie.width, myMovie.height, xOffset, yOffset, (int) (myMovie.width / ratio), (int) (myMovie.height / ratio));
    PImage dithered = createImage(84, 48, RGB);
    int GrayArrayLength = 84 * 48;
    int[] GrayArray = new int[GrayArrayLength];

    for (int x = 0; x < img.width; x++) {
      for (int y = 0; y < img.height; y++) {
        int brightness;
        //brightness = floor(sqrt(brightness(img.get(x, y)))*sqrt(255));
        brightness = floor(brightness(img.get(x, y)));
        GrayArray[x + y * 84] = brightness;
      }
    }

    for (int x = 0; x < img.width; x++) {
      for (int y = 0; y < img.height; y++) {
        int idx = x + y * 84;
        int brightness = GrayArray[idx];
        int error;
        if (brightness > 128) {
          GrayArray[idx] = 1;
          dithered.set(x, y, color(213, 227, 194));
          error = (brightness - 255) / 8;
        } 
        else {
          GrayArray[idx] = 0;
          dithered.set(x, y, color(0));
          error = brightness / 8;
        }
        if ((idx + 1) < GrayArrayLength) {
          GrayArray[idx + 1] += error;
        }
        if ((idx + 2) < GrayArrayLength) {
          GrayArray[idx + 2] += error;
        }
        if ((idx + 84 - 1) < GrayArrayLength) {
          GrayArray[idx + 84 - 1] += error;
        }
        if ((idx + 84) < GrayArrayLength) {
          GrayArray[idx + 84] += error;
        }
        if ((idx + 84 + 1) < GrayArrayLength) {
          GrayArray[idx + 84 + 1] += error;
        }
        if ((idx + 84 * 2) < GrayArrayLength) {
          GrayArray[idx + 84 * 2] += error;
        }
      }
    }

    image(myMovie, 
    0, 48 * 4 + 20, 2 * (myMovie.width / ratio), 2 * (myMovie.height / ratio));
    image(img, 
    0, 48*2+10, 84 * 2, 48 * 2);
    image(dithered, 
    0, 0, 84 * 2, 48 * 2);
    //fill(255);
    //ellipseMode(CENTER);
    //ellipse(mouseX, mouseY, 10, 10);
  }
}
