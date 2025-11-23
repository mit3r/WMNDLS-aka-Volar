#include <ESP8266WiFi.h>
#include <FastLED.h>
#include <espnow.h>

#include "NonVolatile.cpp"
#include "protocol.h"

#define LED_PIN 2     // GPIO2
#define BUTTON_PIN 3  // GPIO3 (RX)
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

struct structStorage {
  uint8_t deviceId = 0;
};

structStorage storage;
structMessage message;
uint32_t order = 0;
CRGB leds[NUM_LEDS];

bool newMessage = false;

void OnDataRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
  memcpy(&message, incomingData, sizeof(message));
  newMessage = true;
}

void setup() {
  Serial.begin(115200);

  // EPSNOW setup
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) return;
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  // FastLED setup
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(180);
  FastLED.setCorrection(TypicalLEDStrip);
  FastLED.clear();
}

void loop() {
  if (newMessage) {
    newMessage = false;

    // Validate message
    if (message.networkId != NETWORK_ID) return;
    if (message.deviceId != storage.deviceId && message.deviceId != 0) return;
    if (message.ledCount > NUM_LEDS) return;
    if (message.order <= order) return;

    // Process message
    order = message.order;
    memcpy(leds, message.leds, sizeof(CRGB) * NUM_LEDS);
    FastLED.show();

    Serial.print("Received message order: ");
    Serial.println(message.order);
  }

  yield();
}