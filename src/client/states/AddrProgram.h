#include "Program.h"
#include "protocol.h"

#pragma once

static uint8_t channelCounter = 0;

constexpr CRGB channelsColors[8] = {
    CRGB::Red,      // 0
    CRGB::Green,    // 1
    CRGB::White,    // 2
    CRGB::Yellow,   // 3
    CRGB::Cyan,     // 4
    CRGB::Magenta,  // 5
    CRGB::Orange,   // 6
    CRGB::Purple    // 7
};

class AddrProgram : public Program {
  public:
  static AddrProgram program;

  void loop() override {
    fill_solid(Strip::leds, NUM_LEDS, CRGB::Black);
    Strip::leds[0] = CRGB::Blue;
    Strip::leds[NUM_LEDS - 1] = CRGB::Blue;

    Strip::leds[1 + channelCounter] = channelsColors[channelCounter];
  };

  void setup() override {
    channelCounter = (unsigned char)storage.data->channelId;
  };

  void onButtonPress() override {
    channelCounter = (channelCounter + 1) % MAX_CHANNELS;
  };

  void onButtonLongPress() override {
    storage.data->channelId = channelCounter;
    storage.save();
    State::set(TState::STATE_RECV);
  }

  void onButtonLongLongPress() override {
    State::set(TState::STATE_IDLE);
  }
};