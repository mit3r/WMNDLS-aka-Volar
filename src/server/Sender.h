#include <ESP8266WiFi.h>
#include <espnow.h>

#include "protocol.h"

#pragma once

class Sender {
  private:
  static Message message;
  static uint16_t order;
  static bool wait;

  static uint8_t speedCounter;

  static void onDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
    wait = false;
    speedCounter++;
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

  static void loop() {
    EVERY_N_SECONDS(1) {
      Serial.printf("ESP-NOW send speed: %d messages/sec\n", speedCounter);
      speedCounter = 0;
    }
  }

  static bool sendMessage(Message message) {
    if (wait) return false;
    // message.header.order = ++order;
    esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&message, message.header.length);
    wait = true;
    return true;
  }

  static bool sendOrderMessage() {
    if (wait) return false;
    // message.header.order = order;
    message.header.type = MessageType::ORDER;
    message.header.length = sizeof(Header);

    esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&message, message.header.length);
    return wait = true;
  }
};

Message Sender::message = {};
uint16_t Sender::order = 0;
bool Sender::wait = false;

uint8_t Sender::speedCounter = 0;