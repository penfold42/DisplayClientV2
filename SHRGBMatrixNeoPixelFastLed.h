#ifndef __SHRGBMATRIXNEOPIXELFASTLED_H__
#define __SHRGBMATRIXNEOPIXELFASTLED_H__

#ifndef FASTLED_ALLOW_INTERRUPTS
#define FASTLED_ALLOW_INTERRUPTS 0
#endif

#include <Arduino.h>
#include "SHRGBLedsBase.h"
#include <FastLED.h>

CRGB SHRGBMatrixNeoPixelFastLeds_leds[64];

class SHRGBMatrixNeoPixelFastLed : public SHRGBLedsBase {
private:
	unsigned long lastRead = 0;

public:

	void begin(int maxLeds, int righttoleft, bool testMode) {
		SHRGBLedsBase::begin(maxLeds, righttoleft);
		FastLED.addLeds<NEOPIXEL, WS2812B_MATRIX_DATAPIN>(SHRGBMatrixNeoPixelFastLeds_leds, maxLeds);

		if (testMode > 0) {
			for (int i = 0; i < maxLeds; i++) {
				setPixelColor(i, 255, 0, 0);
			}
		}
		FastLED.show();
	}

	void show() {
		FastLED.show();
		//delay(10);
	}

protected:
	void setPixelColor(uint8_t lednumber, uint8_t r, uint8_t g, uint8_t b) {
		SHRGBMatrixNeoPixelFastLeds_leds[lednumber].setRGB(r, g, b);
	}
};

#endif