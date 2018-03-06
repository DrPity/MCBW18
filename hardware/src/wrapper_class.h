
#ifndef WRAPPER_H
#define WRAPPER_H
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "MAPPING.h"

	const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };



class Wrapper_class{

private:
	int _numberOfPixels;
	int _stripPin;
	Adafruit_NeoPixel* _strip;
	uint8_t _flicker[NUMBEROFPIXELS];
	uint8_t _targetColorR[NUMBEROFPIXELS];
	uint8_t _targetColorG[NUMBEROFPIXELS];
	uint8_t _targetColorB[NUMBEROFPIXELS];
	bool _colorReached[NUMBEROFPIXELS];
	bool _ignorePixel[NUMBEROFPIXELS];

public:
	void setStripColor();
	uint32_t lastPixelColor[NUMBEROFPIXELS];
	int idx[NUMBEROFPIXELS];

	Wrapper_class(int numberOfPixels, int stripPin): _numberOfPixels(numberOfPixels), _stripPin(stripPin){
		_strip = new Adafruit_NeoPixel(_numberOfPixels, _stripPin, NEO_GRB + NEO_KHZ800);

		for (int i = 0; i < _numberOfPixels; i++) {
			_flicker[i] = 0;
			_ignorePixel[i] = false;
			_targetColorR[i] = 0;
			_targetColorG[i] = 0;
			_targetColorB[i] = 0;
			_colorReached[i] = false;
			lastPixelColor[i] = 0;
			idx[i] = 255;
		}
	}


	void init(){
	_strip->show();
	_strip->begin();
	_strip->setBrightness(255);

	}

	void setStripColor(uint8_t r, uint8_t g, uint8_t b){
		for(int i = 0; i<_numberOfPixels; i++){
			_strip->setPixelColor(i, r, g, b);
		}
		// _strip->show();
	}

	void setBrightness(uint8_t brightness){
		_strip->setBrightness(brightness);
		// _strip->show();
	}

	uint32_t getIntColor(uint8_t r, uint8_t g, uint8_t b){
		return _strip->Color(r,g,b);
	}

	int getNumPixels(){
		return _strip->numPixels();
	}

	uint32_t getPixelColor(int i){
		return _strip->getPixelColor(i);
	}

	void setPixelColor(int i, uint8_t r, uint8_t g, uint8_t b){
		r = pgm_read_byte(&gamma8[r]);
		g = pgm_read_byte(&gamma8[g]);
		b = pgm_read_byte(&gamma8[b]);
		_strip->setPixelColor(i, r, g, b);
		// _strip->show();
	}

	void setPixelColor(int i, uint32_t color){
		_strip->setPixelColor(i, color);
	}

	void show(){
		_strip->show();
	}

	void setIgnorePixel(int i, bool ignore){
		_ignorePixel[i] = ignore;
	}

	bool getIgnorePixel(int i){
		return _ignorePixel[i];
	}

	void setTargetColorR(int i, uint8_t red){
		_targetColorR[i] = red;
	}

	uint8_t getTargetColorR(int i){
		return _targetColorR[i];
	}

	void setTargetColorG(int i, uint8_t green){
		_targetColorG[i] = green;
	}

	uint8_t getTargetColorG(int i){
		return _targetColorG[i];
	}

	void setTargetColorB(int i, uint8_t blue){
		_targetColorB[i] = blue;
	}

	uint8_t getTargetColorB(int i){
		return _targetColorB[i];
	}

	void setNewFlickerValue(int i){
		_flicker[i] = (uint8_t) random(0,200);
	}

	uint8_t getCurrentFlickerValue(int i) {
		return _flicker[i];
	}

	void setColorReached(int i, bool status){
		_colorReached[i] = status;
	}

	bool getColorReached(int i) {
		return _colorReached[i];
	}

	void rainbowCycle(int wait) {
	  int j = (millis()/wait)%(256*5);
	  for(int i=0; i < _strip->numPixels(); i++) {
	    _strip->setPixelColor(i, Wheel(((i * 256 / _strip->numPixels()) + j) & 255));
	  }
	  _strip->show();
	}

	void resetIdx(int i){
			idx[i] = -255;
	}

	// Input a value 0 to 255 to get a color value.
	// The colours are a transition r - g - b - back to r.
	uint32_t Wheel(byte WheelPos) {
	  if(WheelPos < 85) {
	   return _strip->Color(WheelPos * 3, 255 - WheelPos * 3, 0);
	  } else if(WheelPos < 170) {
	   WheelPos -= 85;
	   return _strip->Color(255 - WheelPos * 3, 0, WheelPos * 3);
	  } else {
	   WheelPos -= 170;
	   return _strip->Color(0, WheelPos * 3, 255 - WheelPos * 3);
	  }
	}

};
#endif