import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import processing.serial.*; 
import ddf.minim.analysis.*; 
import codeanticode.syphon.*; 
import oscP5.*; 
import netP5.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class pBridge extends PApplet {







  
OscP5 oscP5;
NetAddress myRemoteLocation;


PGraphics canvas;
PImage img;
SyphonClient client;

Serial myPort;

int avColor;
int fadeSpeed = 20;
int potentimeter = 0;
int potentimeterPrev = 0;

public void setup() {
  
  
  oscP5 = new OscP5(this,7000);
  myRemoteLocation = new NetAddress("10.0.1.147",7000);

  println("Available Syphon servers:");
  println(SyphonClient.listServers());
    
  // Create syhpon client to receive frames 
  // from the first available running server: 
  client = new SyphonClient(this);
  for (int i = 0; i < Serial.list().length; ++i) {
      println("[" + i + "]" + Serial.list()[i]);
  }

  myPort  = new Serial(this, "/dev/tty.usbmodem3432041", 57600);
  myPort.clear();
  myPort.bufferUntil('\n');
  
  avColor = color(204, 102, 0);

  // //send Arduino handshake
  // byte [] bytes = {'a'};
  // sendSerial(bytes);
}


public void draw() {
  fill(avColor);
  rect(width/2, 0, width/2, height);
  noFill();
  if (client.newFrame()) {
    canvas = client.getGraphics(canvas);
    image(canvas, 0, 0, width/2, height);
    img = canvas.get();
    avColor = extractColorFromImage(img);
    // println(avColor);
    myPort.write("Tt1,1," + avColor + "," + avColor + "," + avColor + "," + fadeSpeed +'\n');
    // rect
  }  
    
}

public int extractColorFromImage(PImage img) {
    img.loadPixels();
    int r = 0, g = 0, b = 0;
    for (int i=0; i<img.pixels.length; i++) {
        int c = img.pixels[i];
        r += c>>16&0xFF;
        g += c>>8&0xFF;
        b += c&0xFF;
    }
    r /= img.pixels.length;
    g /= img.pixels.length;
    b /= img.pixels.length;
 
    return color(r, g, b);
}


public void serialEvent( Serial myPort ) {

  while ( myPort.available() > 0 ) {
      String inByte = myPort.readStringUntil('\n').trim();      
      potentimeter = PApplet.parseInt(inByte); 
      if (potentimeter != potentimeterPrev){
        float value = map(potentimeter, 0, 1023, 0, 1);
        OscMessage myMessage = new OscMessage("composition/selectedclip/transport/position");
        myMessage.add(value); /* add an int to the osc message */
        oscP5.send(myMessage, myRemoteLocation);
        println(value);
      }
      potentimeterPrev = potentimeter;
  }
  
}

public void sendSerial( byte[] bytes ) {
  for (int i = 0; i < bytes.length; ++i) {
      myPort.write(bytes[i]);
  }
  myPort.write('\n');
}
  public void settings() {  size(960, 340, P3D); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "pBridge" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
