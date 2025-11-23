#include <ESP8266WiFi.h>
#include <espnow.h>

#include "protocol.h"

uint8_t BROADCAST_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
structMessage message;
uint32_t order = 0;

// Callback when data is sent
void handleDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void handleSend() {
  esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&message, sizeof(message));
  ++message.order;

  Serial.print("Sent message order: ");
  Serial.println(message.order);
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) message.leds[i].nscale8(250);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(handleDataSent);

  esp_now_add_peer(BROADCAST_ADDRESS, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Prepare message
  message.networkId = NETWORK_ID;
  message.deviceId = 0;  // Broadcast to all devices
  message.order = 0;
  message.ledCount = NUM_LEDS;
}

void loop() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    message.leds[i] = CHSV(hue++, 255, 255);
    handleSend();

    fadeall();
    delay(10);
  }

  for (int i = (NUM_LEDS)-1; i >= 0; i--) {
    message.leds[i] = CHSV(hue++, 255, 255);
    handleSend();

    fadeall();
    delay(10);
  }
}