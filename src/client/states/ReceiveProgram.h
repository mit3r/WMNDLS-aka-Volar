#include "Program.h"

#pragma once

class ReceiveProgram : public Program {
  public:
  void setup() override {
    blinks = 0;
    fill_solid(Strip::leds, NUM_LEDS, CRGB::Black);
  }

  void loop() override {
    EVERY_N_SECONDS(1) {
      Serial.printf("Received: %d messages in the last second\n", messagesCounter);
      messagesCounter = 0;

      Serial.printf("Button: %s\n", digitalRead(3) == LOW ? "Pressed" : "Released");
    }

    if (blinks >= 3) return;
    EVERY_N_MILLIS(200) {
      Strip::leds[0] = lightOn ? CRGB::Black : CRGB::White;
      Strip::leds[NUM_LEDS - 1] = lightOn ? CRGB::Black : CRGB::White;
      lightOn = !lightOn;
      if (!lightOn) blinks++;
    }
  }

  void onButtonPress() override {
    blinks = 0;
  }

  void onMessage(const Message& message) override {
    if (blinks < 3) blinks = 3;

    if (message.header.type == MessageType::UPDATE_LEDS) {
      // Serial.printf("Updating %d LEDs\n", message.payloadUpdateLeds.ledCount);
      messagesCounter++;

      memcpy(Strip::leds, message.payloadUpdateLeds.leds, sizeof(CRGB) * NUM_LEDS);
    }
  }

  void onButtonLongPress() override {
    State.set(TState::STATE_ADDR);
  }

  void onButtonLongLongPress() override {
    State.set(TState::STATE_IDLE);
  }

  unsigned char blinks = 0;
  bool lightOn = false;

  unsigned int messagesCounter = 0;
};