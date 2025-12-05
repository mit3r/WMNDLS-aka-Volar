#include <FastLED.h>

#ifndef PROTOCOL_H
#define PROTOCOL_H

// Hardware
#define NUM_LEDS 50
#define MAX_CHANNELS 16  // 0-14 = channels, 15 = broadcast
#define BROADCAST_CHANNEL 0xFF  // All channels

// Communication
#define FPS 30
#define FRAME_DELAY_MS (1000 / FPS)

// ESP-NOW
uint8_t BROADCAST_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define NETWORK_ID 0xCAFEBE16

enum MessageType : uint8_t {
  ORDER,     // Synchronize message order
  CONFIG,    // Configure device
  COLORS24,  // Set LED colors in 24-bit RGB
  COLORS16,  // Set LED colors in 16-bit RGB565
  COLORS8,   // Set LED colors in 8-bit RGB332
};

struct Header {
  uint32_t networkId = NETWORK_ID;  // Network identifier
  uint16_t order = 0;               // Message order to prevent old messages
  uint8_t length = 0;               // Total message length in bytes
  MessageType type;                 // Message type
  uint8_t channels;                 // Every bit represents a destination channel (max 8)
};

struct RGB565 {  // Size: 2B
  uint16_t red : 5;
  uint16_t green : 6;
  uint16_t blue : 5;
};

struct RGB8 {  // Size: 1B
  uint8_t red : 3;
  uint8_t green : 3;
  uint8_t blue : 2;
};

struct Config {
  enum RGBResolution {
    RGB24,
    RGB16,
    RGB8,
  } resolution;
};

struct Message {
  Header header;

  union {
    CRGB colors24[1][NUM_LEDS];
    RGB565 colors16[2][NUM_LEDS];
    RGB8 colors8[4][NUM_LEDS];
    Config config;
  };
};

const size_t MESSAGE_SIZE = sizeof(Message);
const size_t HEADER_SIZE = sizeof(Header);

CRGB RGB16to24(RGB565 color) {
  CRGB color24 = {
      static_cast<uint8_t>((color.red << 3) | (color.red >> 2)),
      static_cast<uint8_t>((color.green << 2) | (color.green >> 4)),
      static_cast<uint8_t>((color.blue << 3) | (color.blue >> 2)),
  };
  return color24;
}

CRGB RGB8to24(RGB8 color) {
  CRGB color24 = {
      static_cast<uint8_t>((color.red << 5) | (color.red << 2) | (color.red >> 1)),
      static_cast<uint8_t>((color.green << 5) | (color.green << 2) | (color.green >> 1)),
      static_cast<uint8_t>((color.blue << 6) | (color.blue << 4) | (color.blue << 2) | color.blue),
  };
  return color24;
}

#endif  // PROTOCOL_H