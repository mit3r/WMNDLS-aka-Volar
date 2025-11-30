#include <FastLED.h>

#include "protocol.h"

#pragma once

#define DATA_PIN 2  // GPIO2
#define NUM_LEDS 50

class Strip {
  public:
  static CRGB leds[NUM_LEDS];

  static void setup() {
    // FastLED setup
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(Strip::leds, NUM_LEDS);
    FastLED.setBrightness(180);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.clear();
  }
};

CRGB Strip::leds[NUM_LEDS];