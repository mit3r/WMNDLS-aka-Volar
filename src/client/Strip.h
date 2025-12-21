#include <FastLED.h>

#include "protocol.h"

#pragma once

#define DATA_PIN 2  // GPIO2
#define NUM_LEDS 50

class Strip {
  static void set(const CRGB colors[NUM_LEDS]) {
    memcpy(Strip::leds, colors, sizeof(CRGB) * NUM_LEDS);
    changed = true;
  };

  static void set(const RGB565 colors[NUM_LEDS]) {
    for (size_t i = 0; i < NUM_LEDS; i++)
      Strip::leds[i] = RGB16to24(colors[i]);
    changed = true;
  };

  static void set(const RGB8 colors[NUM_LEDS]) {
    for (size_t i = 0; i < NUM_LEDS; i++)
      Strip::leds[i] = RGB8to24(colors[i]);
    changed = true;
  };

  public:
  static void
  setup() {
    // FastLED setup
    FastLED.addLeds<WS2812, DATA_PIN, GRB>(Strip::leds, NUM_LEDS);
    FastLED.setBrightness(180);
    FastLED.setCorrection(TypicalLEDStrip);
    FastLED.clear();
  }

  static void onMessage(Message* message, uint8_t* length) {
    Serial.println("Strip::onMessage called");

    if (message->header.type != MessageType::COLORS8 &&
        message->header.type != MessageType::COLORS16 &&
        message->header.type != MessageType::COLORS24) {
      return;
    }

    Serial.printf("Proccesing new message for channels:");
    for (uint8_t i = 0; i <= 7; i++)
      if (message->header.channels & (1 << i)) Serial.printf(" %d", i);
    Serial.println();

    Serial.printf("My channel ID: %d\n", storage.data->channelId);

    uint8_t payloadId = 0;
    if (message->header.channels != BROADCAST_CHANNEL) {  // Channel

      for (uint8_t id = 0; id <= 7; id++) {
        if (id == storage.data->channelId) break;
        if (message->header.channels & (1 << id)) payloadId++;
      }
    }

    Serial.printf("Payload ID: %d\n", payloadId);

    if (message->header.type == MessageType::COLORS8) Strip::set(message->colors8[payloadId]);
    if (message->header.type == MessageType::COLORS16) Strip::set(message->colors16[payloadId]);
    if (message->header.type == MessageType::COLORS24) Strip::set(message->colors24[payloadId]);

    Serial.printf("Updated strip with %d colors of type %d on channel %d\n",
                  NUM_LEDS, message->header.type, storage.data->channelId);

    changed = true;
  }

  static void loop() {
    // if (changed && (changed = false)) FastLED.show();
    FastLED.show();
  }

  static CRGB leds[NUM_LEDS];
  static bool changed;
};

CRGB Strip::leds[NUM_LEDS];
bool Strip::changed = false;