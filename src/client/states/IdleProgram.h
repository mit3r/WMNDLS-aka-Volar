#include <protocol.h>

#include <algorithm>

#include "../Strip.h"
#include "Program.h"

// static unsigned int

class IdleProgram : public Program {
  private:
  public:
  void setup() override {
    blinks = 0;
    fill_solid(Strip::leds, NUM_LEDS, CRGB::Black);
  }

  void loop() override {
    if (blinks >= 3) return;
    EVERY_N_MILLIS(200) {
      Strip::leds[0] = lightOn ? CRGB::Black : CRGB::Yellow;
      Strip::leds[NUM_LEDS - 1] = lightOn ? CRGB::Black : CRGB::Yellow;
      lightOn = !lightOn;
      if (!lightOn) blinks++;
    }
  }

  void onButtonPress() override {
    blinks = 0;
  }

  void onButtonLongPress() override {
    State::set(TState::STATE_RECV);
  }

  void onButtonLongLongPress() override {
    State::set(TState::STATE_RECV);
  }

  unsigned char blinks = 0;
  bool lightOn = false;

} idleProgram;
