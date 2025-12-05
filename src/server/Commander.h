#include <Arduino.h>
#include <FastLED.h>

#include "protocol.h"

#pragma once

static uint16_t counter = 0;

class Commander {
  // Buffer
  static uint8_t bufi;
  static uint8_t buffer[sizeof(Message)];

  static bool msgReady;

  static bool isValidCommand() {
    Message* msg = (Message*)buffer;

    if (msg->header.channels == 0) return false;  // No channels specified
    if (msg->header.type != MessageType::ORDER &&
        msg->header.type != MessageType::CONFIG &&
        msg->header.type != MessageType::COLORS24 &&
        msg->header.type != MessageType::COLORS16 &&
        msg->header.type != MessageType::COLORS8)
      return false;  // Invalid message type

    return true;
  }

  public:
  static void setup() {
    Serial.begin(115200);
  }

  static void loop() {
    EVERY_N_SECONDS(1) {
      Serial.printf("Received commands: %d\n", counter);
      counter = 0;
    }

    while (Serial.available() > 0) {
      uint8_t byte = Serial.read();
      buffer[bufi++] = byte;

      // Wait until we have at least the network ID
      if (bufi < sizeof(Message::header.networkId)) continue;

      // Verify network ID
      if (((Message*)buffer)->header.networkId != NETWORK_ID) {
        memmove(buffer, buffer + 1, --bufi);
        continue;
      }

      // Wait until we have length info
      if (bufi <= sizeof(Message::header)) continue;

      // Serial.printf("bufi: %d, length: %d\n", bufi, ((Message*)buffer)->header.length);

      if (((Message*)buffer)->header.length == bufi) {
        bufi = 0;
        counter++;
        msgReady = true;
        continue;
      }
    }
  }

  static bool hasMessage() {
    return msgReady;
  }

  static Message popMessage() {
    msgReady = false;
    Message* msg = (Message*)buffer;
    return *msg;
  }
};

uint8_t Commander::bufi = 0;
uint8_t Commander::buffer[sizeof(Message)] = {0};

bool Commander::msgReady = false;