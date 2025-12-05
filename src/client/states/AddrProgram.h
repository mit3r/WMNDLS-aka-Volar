#include "Program.h"
#include "protocol.h"

#pragma once

#define MAN_ADDR_COUNT 16
static uint8_t counter = MAN_ADDR_COUNT - 1;

class AddrProgram : public Program {
  public:
  static AddrProgram program;

  void loop() override {
    fill_solid(Strip::leds, NUM_LEDS, CRGB::Black);
    Strip::leds[0] = CRGB::Blue;
    Strip::leds[NUM_LEDS - 1] = CRGB::Blue;

    for (size_t i = 0; i < log2(MAN_ADDR_COUNT); i++) {
      if (counter & (1 << i)) Strip::leds[i + 1] = CRGB::Red;
    }
  };

  void setup() override {
    counter = (unsigned char)storage.channelId;
  };

  void onButtonPress() override {
    counter = (counter + 1) % MAN_ADDR_COUNT;
  };

  void onButtonLongPress() override {
    storage.channelId = counter;
    State::set(TState::STATE_RECV);
  }

  void onButtonLongLongPress() override {
    State::set(TState::STATE_IDLE);
  }
};