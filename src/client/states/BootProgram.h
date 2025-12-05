#include <client/State.h>
#include <client/Strip.h>

#include "Arduino.h"
#include "Program.h"

#pragma once

class BootProgram : public Program {
  public:
  void setup() override {
    blinks = 0;
    fill_solid(Strip::leds, NUM_LEDS, CRGB::Black);
  }

  void loop() override {
    EVERY_N_MILLIS(200) {
      Strip::leds[0] = lightOn ? CRGB::Black : CRGB::Green;
      Strip::leds[NUM_LEDS - 1] = lightOn ? CRGB::Black : CRGB::Green;
      lightOn = !lightOn;
      if (!lightOn) blinks++;
      if (blinks == 1) State::set(TState::STATE_RECV);
    }
  }

  unsigned long blinks = 0;
  bool lightOn = false;
};