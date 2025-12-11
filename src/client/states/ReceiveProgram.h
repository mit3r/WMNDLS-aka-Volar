#include "Program.h"

#pragma once

class ReceiveProgram : public Program {
  public:
  void setup() override {
    blinks = 0;
    fill_solid(Strip::leds, NUM_LEDS, CRGB::Black);
    Serial.println("Entered Receive Program");
  }

  void loop() override {
    if (blinks >= 3) return;
    EVERY_N_MILLIS(200) {
      Strip::leds[0] = lightOn ? CRGB::Black : CRGB::White;
      Strip::leds[NUM_LEDS - 1] = lightOn ? CRGB::Black : CRGB::White;
      lightOn = !lightOn;
      if (!lightOn) blinks++;
      Strip::changed = true;
    }
  }

  void onButtonPress() override {
    blinks = 0;
  }

  void onButtonLongPress() override {
    State::set(TState::STATE_ADDR);
  }

  void onButtonLongLongPress() override {
    State::set(TState::STATE_IDLE);
  }

  unsigned char blinks = 0;
  bool lightOn = false;
};