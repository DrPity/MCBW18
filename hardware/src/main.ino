///////REMEMBER TO SET THIS FUCKING VALUES

#include <Adafruit_NeoPixel.h>
#include "wrapper_class.h"
#include "stripSegments.h"
#include <Encoder.h>
#include "MAPPING.h"


char lf                    = '\n';

bool positionIsNotRequested = false;
bool shallFlickerInFade     = true;
bool watchdogActive         = false;
bool colorReached           = true;
bool shallFlicker           = true;
bool isFlickering           = false;
bool startRainbow           = false;
bool isSegments             = false;
bool ledsReady              = true;
bool setStrip               = false;

long parameterArray[6];
long splitArray[6];

int connectionTimeOut =  10;
int fadeOutSpeed      = 2;
int offset = 0;

long currentTime[8];
long waitTime[8];
long oldPosition  = -999;

bool fall = false;
bool rise = true;

uint8_t lastPixel = 0;
uint8_t lastR,lastG,lastB = 0;

uint32_t lastPixelColor[NUMBEROFPIXELS];

int rc = 0;
int gc = 0;
int bc = 0;
int rt = 0;
int gt = 0;
int bt = 0;
int fade = 0;

unsigned long loopTime;

uint8_t fadeSpeed        = 1;

String inByte;

float filter = 0.10;
int potentiometerPrev = 0;

Encoder myEnc(ENCODER_1, ENCODER_2);

Wrapper_class strips[] = {
  Wrapper_class(NUMBEROFPIXELS, STRIP_PIN_1),
};

StripSegments segments[] = {
  StripSegments(0,3),
};

//------------------------------------------------------------------------------

void setup() {
  Serial.begin(57600);
  for (int i = 0; i < NUMSTRIPS; ++i)
  {
    strips[i].init();
  }
  // Serial.println("Setup");
  wait(1,0);
}


//------------------------------------------------------------------------------

void loop() {

  if (Serial.available() > 0){
    inByte = Serial.readStringUntil(lf);
    inByte.trim();
    // Serial.println(inByte);

    if(inByte.indexOf('T') == 0 && inByte.indexOf('t') == 1){
      setTargetColor(inByte);
    }
    if(inByte.indexOf('C') == 0 && inByte.indexOf('c') == 1){
      setColor(inByte);
    }
    if(inByte.indexOf('P') == 0 && inByte.indexOf('p') == 1){
      offset = 0;
    }
    // if(inByte.indexOf('#') == 0){
    //   // Serial.print("master");   // send a capital A
    //   // Serial.println();
    // }


  }

  if(startRainbow){
    if(millis() - loopTime >= 60){
      for (int i = 0; i < NUMSTRIPS; i++){
        strips[i].rainbowCycle(20);
      }
      loopTime = millis();
    }
  }


  if(fadeSpeed > 0 && checkTimers(0)){
    setStrip = true;
    for(int i = 0; i < NUMSTRIPS; i++){
      fadeOut(i);
    }
  }


  if(shallFlicker){
    flicker(0);
    setStrip = true;
  }
  //
  //
  if(checkTimers(1)){
    for (int i = 0; i < NUMSTRIPS; i++){
      int k = random(offset,offset+5);
      if(k < NUMBEROFPIXELS){
        strips[i].resetIdx(k);
        offset++;
      }else if (k > NUMBEROFPIXELS){
        offset = NUMBEROFPIXELS +1;
      }
    }
    wait(20,1);
  }

  if(setStrip){
    showStrips();
    setStrip = false;
  }


  if(checkTimers(2)){
    long newPosition = myEnc.read();
    if (newPosition != oldPosition) {
      if(newPosition > oldPosition){
        Serial.print(1);  
      }else if (newPosition < oldPosition){
        Serial.print(-1); 
      }
      Serial.print("\n");
      oldPosition = newPosition;
    }
    // int potentiometer = (1-filter) * potentiometerPrev + filter * analogRead(POTI);
    // if (abs(potentiometer - potentiometerPrev) >= 1){
    //   Serial.print(potentiometer);
    //   Serial.print("\n");
    // }
    // potentiometerPrev = potentiometer;
    wait(50,2);
  }
}

//------------------------------------------------------------------------------

void showStrips(){
  for(int i = 0; i < NUMSTRIPS; i++){
    strips[i].show();
  }
}

//------------------------------------------------------------------------------

void setColor(String inByte){

  for (int i = 0; i < 5; i++){
    if(i == 0){
      splitArray[i] = inByte.indexOf(',');
      parameterArray[i] = inByte.substring(2,splitArray[i]).toInt();
    }else if(i > 0){
      splitArray[i] = inByte.indexOf(',', splitArray[i-1] + 1);
      parameterArray[i] = inByte.substring(splitArray[i-1] +1,splitArray[i]).toInt();
    }
  }

  int strip      = (int) parameterArray[0];
  rc             = (int) parameterArray[1];
  gc             = (int) parameterArray[2];
  bc             = (int) parameterArray[3];
  // colorReached   = parameterArray[4];

  // Serial.println(strip);
  // Serial.println(rc);
  // Serial.println(gc);
  // Serial.println(bc);
  // Serial.println(colorReached);

  positionIsNotRequested = true;
  if(strip == 0){
    for(int i = 0; i < NUMSTRIPS; i++){
      strips[i].setStripColor(rc,gc,bc);
    }
  }

  if(strip > 0 && strip < 9){
    strips[strip - 1].setStripColor(rc,gc,bc);
  }

  if(strip == 9){
    // Serial.print("Rainbow set to true");
    startRainbow = true;
    loopTime = millis();
  }

  if(strip == 10){
    startRainbow = false;
    for(int i = 0; i < NUMSTRIPS; i++){
      strips[i].setStripColor(rc,gc,bc);
    }
  }

  setStrip = true;
  // positionIsNotRequested = true;
}

//------------------------------------------------------------------------------

void setTargetColor(String inByte){
  for (int i = 0; i < 6; i++){
    if(i == 0){
      splitArray[i] = inByte.indexOf(',');
      parameterArray[i] = inByte.substring(2,splitArray[i]).toInt();
    }else if(i > 0){
      splitArray[i] = inByte.indexOf(',', splitArray[i-1] + 1);
      parameterArray[i] = inByte.substring(splitArray[i-1] +1,splitArray[i]).toInt();
    }
  }


  int strip      = (int) parameterArray[0];
  int segment    = (int) parameterArray[1];

  long color[3];
  color[0]   = parameterArray[2];
  color[1]   = parameterArray[3];
  color[2]   = parameterArray[4];

  fade       = (int) parameterArray[5];

  // Serial.println("color params are: ");
  // Serial.println(color[0]);
  // Serial.println(color[1]);
  // Serial.println(color[2]);

  // isSegments = false;


  // to set all strips send: Tt0,0,color,emptyColor,emptyColor,fade
  // to set a strip send: Tt1,0,color,emptyColor,emptyColor,fade
  // to set a segment send: Tt1,1,color,color,color,fade
  if(strip == 1 && segment == 1){
    for(int k = 0; k < NUMSTRIPS; k++){
      for(int s = 0; s < STRIPSEGMENTS; s++){
        for(int i = segments[s].lowerBound; i < segments[s].upperBound; i++){
          int
          rt = (uint8_t)(color[s] >> 16),
          gt = (uint8_t)(color[s] >>  8),
          bt = (uint8_t)color[s];

          strips[k].setTargetColorR(i,rt);
          strips[k].setTargetColorG(i,gt);
          strips[k].setTargetColorB(i,bt);
          strips[k].setColorReached(i,false);

        }
      }
    }
  }

  colorReached = false;
  // positionIsNotRequested = true;
  // Serial.println(millis());
}

//------------------------------------------------------------------------------

void fadeOut(int k){
  // Serial.println("In fade");
  for(int i = 0; i < strips[k].getNumPixels(); i++){

    // strips[k].lastPixelColor[i] = strips[k].getPixelColor(i);

    // uint32_t color = strips[k].getPixelColor(i);

    int
    r = (uint8_t)(strips[k].lastPixelColor[i] >> 16),
    g = (uint8_t)(strips[k].lastPixelColor[i] >>  8),
    b = (uint8_t)strips[k].lastPixelColor[i];

    // Serial.println(r);
    // Serial.println(g);
    // Serial.println(b);

    r = (fadeSpeed >= abs(strips[k].getTargetColorR(i) - r))?strips[k].getTargetColorR(i):r<strips[k].getTargetColorR(i)?r+fadeSpeed:r-fadeSpeed;
    g = (fadeSpeed >= abs(strips[k].getTargetColorG(i) - g))?strips[k].getTargetColorG(i):g<strips[k].getTargetColorG(i)?g+fadeSpeed:g-fadeSpeed;
    b = (fadeSpeed >= abs(strips[k].getTargetColorB(i) - b))?strips[k].getTargetColorB(i):b<strips[k].getTargetColorB(i)?b+fadeSpeed:b-fadeSpeed;

    // if(r == strips[k].getTargetColorR(i) && g == strips[k].getTargetColorG(i) && b == strips[k].getTargetColorB(i)){
    //   strips[k].setColorReached(i,true);
    // }

    strips[k].lastPixelColor[i] = strips[k].getIntColor(r,g,b);
    if(shallFlickerInFade && checkTimers(3)){
      strips[k].setBrightness(random(135,255));
      wait(50,3);
    }
    strips[k].setPixelColor(i, r, g, b);
    wait(fade,0);
  }

  if(checkPixelColor(k)){
    // Serial.println("Truuue");
    colorReached = true;
    // isSegments = false;
  }
}

//------------------------------------------------------------------------------

bool checkSegColor()
{
  for(int s = 0; s < STRIPSEGMENTS-1; s++){
    if(segments[s].colorReached != segments[s + 1].colorReached){
      return false;
    }else if(!segments[s].colorReached && !segments[s + 1].colorReached){
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------

bool checkPixelColor(int k)
{
  for(int i = 0; i < strips[k].getNumPixels()-1; i++){
    // if(strips[k].getColorReached(i) != strips[k].getColorReached(i+1)){
    //   return false;
    // }else
    if(!strips[k].getColorReached(i)){
      return false;
    }
  }
  return true;
}

//------------------------------------------------------------------------------

void flicker(int k){
  for(int i = 0; i < NUMBEROFPIXELS; i++)
  {
    uint32_t color = strips[k].lastPixelColor[i];
    int
    r = (uint8_t)(color >> 16),
    g = (uint8_t)(color >>  8),
    b = (uint8_t)color;
    //One need to iterate up

    r = interpolate(255,r,strips[k].idx[i],255);
    g = interpolate(255,g,strips[k].idx[i],255);
    b = interpolate(255,b,strips[k].idx[i],255);


    if(strips[k].idx[i] >= 255){
      strips[k].idx[i] = 255;
    }else{
      strips[k].idx[i] += 1;
    }

    strips[k].setPixelColor(i,r,g,b);
  }
}

int interpolate(int startValue, int endValue, int stepNumber, int lastStepNumber)
{
  return (endValue - startValue) * abs(stepNumber) / lastStepNumber + startValue;
}


//------------------------------------------------------------------------------

void wait(long _waitTime, long _numberOfIndex)
{
  waitTime[_numberOfIndex] = _waitTime;
  currentTime[_numberOfIndex] = millis();
}

//------------------------------------------------------------------------------

bool checkTimers(int _index)
{
  if(millis() - currentTime[_index] >= waitTime[_index]){
    currentTime[_index] = millis();
    return true;
  }else
  {
    return false;
  }
}