
import processing.serial.*;
import ddf.minim.analysis.*;
import codeanticode.syphon.*;
import oscP5.*;
import netP5.*;
  
OscP5 oscP5;
NetAddress myRemoteLocation;


PGraphics canvas;
PImage img;
SyphonClient client;

Serial myPort;

color avColor;
int fadeSpeed = 20;
int potentimeter = 0;
int potentimeterPrev = 0;

int direction = 1;
boolean frameReached = false;
boolean timedOut = true;

long activeTime = 0;

int upperBound = 1024;
int frameCounter = 0;
int idx = 0;

void setup() {
  size(960, 340, P3D);
  
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


void draw() {
  fill(avColor);
  rect(width/2, 0, width/2, height);
  noFill();
  if (client.newFrame()) {
    canvas = client.getGraphics(canvas);
    image(canvas, 0, 0, width/2, height);
    img = canvas.get();
    // avColor = extractColorFromImage(img);
    // // println(avColor);
    // myPort.write("Tt1,1," + avColor + "," + avColor + "," + avColor + "," + fadeSpeed +'\n');
    // rect
  } 
  
  
  if (millis() - activeTime >= 300000){
    timedOut = true;
    OscMessage myMessage = new OscMessage("/layer3/clip2/connect");
    myMessage.add(1); /* add an int to the osc message */
    oscP5.send(myMessage, myRemoteLocation);
    avColor = color(50,50,50);
    // println(avColor);
    myPort.write("Tt1,1," + avColor + "," + avColor + "," + avColor + "," + fadeSpeed +'\n');
  }
  
  if (direction == 1 && !frameReached){
    idx++;
    if (idx >= 20){
      frameReached = true;
      OscMessage myMessage = new OscMessage("/activeclip/audio/position/direction");
      myMessage.add(1);
      oscP5.send(myMessage, myRemoteLocation);
    }
    frameCounter++;
    if (frameCounter >= upperBound){
      frameCounter = 0;
    }
    float value = map(frameCounter, 0, upperBound, 0, 1);
    OscMessage myMessage = new OscMessage("/activeclip/audio/position/values");
    myMessage.add(value); /* add an int to the osc message */
    oscP5.send(myMessage, myRemoteLocation);
  }
  
  
  if (direction == -1 && !frameReached){
    idx++;
    if (idx >= 20){
      frameReached = true;
      OscMessage myMessage = new OscMessage("/activeclip/audio/position/direction");
      myMessage.add(0);
      oscP5.send(myMessage, myRemoteLocation);
    }
    frameCounter--;
    if (frameCounter <= 0){
      frameCounter = upperBound;
    }
    float value = map(frameCounter, 0, upperBound, 0, 1);
    OscMessage myMessage = new OscMessage("/activeclip/audio/position/values");
    myMessage.add(value); /* add an int to the osc message */
    oscP5.send(myMessage, myRemoteLocation);
  }
    
}

color extractColorFromImage(PImage img) {
    img.loadPixels();
    int r = 0, g = 0, b = 0;
    color c = img.pixels[(int)random(0,img.pixels.length)];
    r += c>>16&0xFF;
    g += c>>8&0xFF;
    b += c&0xFF;
    // for (int i=0; i<img.pixels.length; i++) {
    //     color c = img.pixels[i];
    //     r += c>>16&0xFF;
    //     g += c>>8&0xFF;
    //     b += c&0xFF;
    // }
    // r /= img.pixels.length;
    // g /= img.pixels.length;
    // b /= img.pixels.length;
 
    return color(r, g, b);
}


void serialEvent( Serial myPort ) {

  while ( myPort.available() > 0 ) {
      String inByte = myPort.readStringUntil('\n').trim();      
      int encoder = int(inByte);
      println(encoder);
      direction = encoder;
      activeTime = millis();
      if (timedOut){
        OscMessage myMessage = new OscMessage("/layer3/clip1/connect");
        myMessage.add(1); /* add an int to the osc message */
        oscP5.send(myMessage, myRemoteLocation);
        timedOut = false;
      }
      OscMessage myMessage = new OscMessage("/activeclip/audio/position/direction");
      myMessage.add(2); /* add an int to the osc message */
      oscP5.send(myMessage, myRemoteLocation);
      idx = 0;
      frameReached = false;
      avColor = color(random(120)+135, random(120)+135, random(120)+135);
      // println(avColor);
      myPort.write("Tt1,1," + avColor + "," + avColor + "," + avColor + "," + fadeSpeed +'\n');
      // potentimeter = int(inByte); 
      // if (potentimeter != potentimeterPrev){
      //   float value = map(potentimeter, 0, 1023, 0, 1);
      //   OscMessage myMessage = new OscMessage("composition/selectedclip/transport/position");
      //   myMessage.add(value); /* add an int to the osc message */
      //   oscP5.send(myMessage, myRemoteLocation);
      //   println(value);
      // }
      // potentimeterPrev = potentimeter;
  }
  
}

void sendSerial( byte[] bytes ) {
  for (int i = 0; i < bytes.length; ++i) {
      myPort.write(bytes[i]);
  }
  myPort.write('\n');
}
