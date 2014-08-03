/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package toolkit;

import java.io.IOException;
import javax.imageio.ImageIO;
import java.io.File;
import java.awt.image.*;

/**
 *
 * @author Rodot
 */
public class BitmapEncoder {

    BufferedImage inputImage;
    BufferedImage outputImage;
    int encodedData[];
    String header = "const byte myBitmap[] PROGMEM = {";
    String footer = "};";
    String output = "";
    boolean hexFormatting = false;
    boolean wrapping = true;

    void BitmapEncoder() {
    }

    void open(File file) {
        try {
            inputImage = ImageIO.read(file);
        } catch (IOException e) {
            e.printStackTrace();
        }
        if (inputImage != null) {
            if ((inputImage.getWidth() > 200) || (inputImage.getHeight() > 200)){
                inputImage = null;
                return;
            }
            outputImage = deepCopy(inputImage);
        }
    }

    void threshold(int thres) {
        if (inputImage == null) {
            return;
        }
        for (int y = 0; y < inputImage.getHeight(); y++) {
            for (int x = 0; x < inputImage.getWidth(); x++) {
                int rgb = inputImage.getRGB(x, y);
                int red = (rgb >> 16) & 0x000000FF;
                int green = (rgb >> 8) & 0x000000FF;
                int blue = (rgb) & 0x000000FF;
                int value = red + green + blue;
                if (value > thres) {
                    outputImage.setRGB(x, y, 0x00FFFFFF);
                } else {
                    outputImage.setRGB(x, y, 0);
                }
            }
        }
    }

    void encode(int thres) {
        if (inputImage == null) {
            return;
        }
        output = "";
        output = output.concat(header);
        int width = ((inputImage.getWidth()-1)/8+1)*8; //round to the closest larger multiple of 8
        output = output.concat(width + "," + inputImage.getHeight() + ",");
        if (wrapping) {
            output = output.concat("\n");
        }
        for (int y = 0; y < inputImage.getHeight(); y++) {
            for (int x = 0; x < inputImage.getWidth(); x += 8) {
                if (hexFormatting) {
                    output = output.concat("0x");
                } else {
                    output = output.concat("B");
                }
                int thisByte = 0;
                for (int b = 0; b < 8; b++) {
                    int value = 0xFFFF;
                    if (x + b < inputImage.getWidth()) {
                        int rgb = inputImage.getRGB(x + b, y);
                        int red = (rgb >> 16) & 0x000000FF;
                        int green = (rgb >> 8) & 0x000000FF;
                        int blue = (rgb) & 0x000000FF;
                        value = red + green + blue;
                    }
                    if (hexFormatting) {
                        thisByte *= 2;
                        if (value < thres) {
                            thisByte++;
                        }
                    } else {//binary formattning
                        if (value < thres) {
                            output = output.concat("1");
                        } else {
                            output = output.concat("0");
                        }

                    }
                }
                if (hexFormatting) {
                    output = output.concat(Integer.toString(thisByte, 16));
                }
                output = output.concat(",");
            }
            if (wrapping) {
                output = output.concat("\n");
            }
        }
        output = output.concat(footer);
    }

    static BufferedImage deepCopy(BufferedImage bi) {
        ColorModel cm = bi.getColorModel();
        boolean isAlphaPremultiplied = cm.isAlphaPremultiplied();
        WritableRaster raster = bi.copyData(null);
        return new BufferedImage(cm, raster, isAlphaPremultiplied, null);
    }
}
