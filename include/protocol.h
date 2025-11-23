#include <FastLED.h>

#ifndef COMMUNITATION_H
#define COMMUNITATION_H

// Randomly chosen network ID
#define NETWORK_ID 0x308B
// Number of LEDs in the strip
#define NUM_LEDS 50

#define MSG_TYPE_NEW_ORDER 0x01    // Reset internal order counter
#define MSG_TYPE_UPDATE_LEDS 0x02  // Update LED colors
#define MSG_TYPE_SHOW 0x03         // Let it show itself (client displays attention pattern)

// Struct message format
struct structMessage {
  uint16_t networkId;   // Network identifier
  uint8_t deviceId;     // Target device identifier
  uint8_t type;         // Command type
  uint32_t order;       // Message index for tracking
  uint16_t ledCount;    // Number of LEDs included in the message
  CRGB leds[NUM_LEDS];  // LED color data
};

#endif  // COMMUNITATION_H