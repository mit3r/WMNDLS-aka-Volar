#include <FastLED.h>

#ifndef COMMUNITATION_H
#define COMMUNITATION_H

// Randomly chosen network ID
#define NETWORK_ID 0x308B
// Number of LEDs in the strip
#define NUM_LEDS 50

enum MessageType : uint8_t {
  NEW_ORDER = 0x01,    // Reset internal order counter
  UPDATE_LEDS = 0x02,  // Update LED colors
  SHOW = 0x03,         // Let it show itself (client displays attention pattern)
  SET_ADDR = 0x04      // Set device address
};

struct Header {
  uint16_t networkId;
  uint8_t deviceId;
  MessageType type;
  uint32_t order;
};

union Message {
  Header header;

  union {
    struct {
      uint8_t newDeviceId;
    } payloadSetAddress;

    struct {
      uint16_t ledCount;
      CRGB leds[NUM_LEDS];
    } payloadUpdateLeds;
  };
};

#endif  // COMMUNITATION_H