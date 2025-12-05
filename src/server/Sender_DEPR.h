
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "protocol.h"

#pragma once

#define BUFFERS_SIZE 6

struct ColorsBuffer {
  uint8_t channelId;
  RGB565 leds[NUM_LEDS];
};

class Sender {
  private:
  static Message message;
  static uint16_t order;
  static bool wait;
  static int packetsCount;

  static ColorsBuffer ledsBuffer[BUFFERS_SIZE];
  static uint8_t buf;

  static void onDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
    wait = false;
    packetsCount++;
  }

  public:
  static void setup() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != 0) {
      Serial.println("Error initializing ESP-NOW");
      return;
    }

    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(onDataSent);

    esp_now_add_peer(BROADCAST_ADDRESS, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  }

  /**
   * Push LED update messages in the send queue
   * @return Number of messages pushed
   */
  static bool handle() {
    if (buf == 0 || wait) return false;

    message.header.order = order++;
    message.header.type = MessageType::COLORS24;

    // Pack messages in pairs
    do {
      for (uint8_t i = 0; i <= buf % 2; i++) {
        auto* buffer = &ledsBuffer[--buf];
        message.header.channels[i] = buffer->channelId;
        // memcpy(message.colors16[i], buffer->leds, sizeof(buffer->leds));
      }

      esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&message, sizeof(Header) + sizeof(RGB565) * NUM_LEDS * (buf % 2 + 1));

    } while (buf > 0);

    return wait = true;
  }

  static uint8_t popPacketsCount() {
    int count = packetsCount;
    packetsCount = 0;
    return count;
  }

  /**
   * Commit LED update to the send queue
   * @param channelId Channel ID
   * @param leds LED colors
   * @return true if committed, false if the buffer is full
   */
  static bool commitLedUpdate(uint8_t channel, CRGB leds[NUM_LEDS]) {
    if (buf >= BUFFERS_SIZE) return false;

    // ledsBuffer[buf].channelId = channel;
    // for (size_t i = 0; i < NUM_LEDS; i++) ledsBuffer[buf].leds[i] = RGB24to16(leds[i]);

    buf++;
    return true;
  }

  static bool pushConfigUpdate(uint8_t channel, uint8_t brightness) {
    if (wait) return false;
    message.header.order = order++;
    message.header.channels[0] = channel;
    message.header.channels[1] = channel;
    message.header.type = MessageType::CONFIG;

    esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&message, sizeof(message));
    return wait = true;
  }

  static bool sendOrderMessage() {
    if (wait) return false;
    message.header.order = order++;
    message.header.channels[0] = BROADCAST_CHANNEL;
    message.header.channels[1] = BROADCAST_CHANNEL;
    message.header.type = MessageType::ORDER;

    esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&message, sizeof(message));
    return wait = true;
  }
};

Message Sender::message = {
    {.networkId = NETWORK_ID,
     .order = 0,
     .type = MessageType::COLORS24,
     .channels = {BROADCAST_CHANNEL, BROADCAST_CHANNEL}}};

ColorsBuffer Sender::ledsBuffer[BUFFERS_SIZE];

uint16_t Sender::order = 0;
uint8_t Sender::buf = 0;
bool Sender::wait = false;
int Sender::packetsCount = 0;