#include <ESP8266WiFi.h>
#include <espnow.h>

#include "protocol.h"

uint8_t BROADCAST_ADDRESS[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static const Header header{
    .networkId = NETWORK_ID,
    .deviceId = 1,
    .order = 0,
};

static Message ledMessage = {header};
static Message orderMessage = {header};
static Message addrMessage = {header};
uint32_t order = 0;

// Callback when data is sent
static unsigned int goodSends = 0;
static unsigned int badSends = 0;
static bool sendDone = true;

void handleDataSent(uint8_t* mac_addr, uint8_t sendStatus) {
  sendDone = true;
  if (sendStatus == 0)
    goodSends++;
  else
    badSends++;
}

bool handleSendUpdate() {
  if (!sendDone) return false;
  sendDone = false;
  esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&ledMessage, sizeof(ledMessage));
  ledMessage.header.order = ++order;
  return true;
}

bool handleSendNewOrder() {
  if (!sendDone) return false;
  sendDone = false;
  esp_now_send(BROADCAST_ADDRESS, (uint8_t*)&orderMessage, sizeof(orderMessage));
  orderMessage.header.order = ++order;
  return true;
}

void fadeall() {
  for (int i = 0; i < NUM_LEDS; i++) ledMessage.payloadUpdateLeds.leds[i].nscale8(250);
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

  ledMessage.payloadUpdateLeds.ledCount = NUM_LEDS;
  ledMessage.header.type = MessageType::UPDATE_LEDS;

  orderMessage.header.type = MessageType::NEW_ORDER;
  orderMessage.header.deviceId = 0;
}

bool firstRun = true;

void loop() {
  if (firstRun) {
    Serial.println("Ressting order");
    firstRun = false;
    for (int i = 0; i < 5; i++)
      while (!handleSendNewOrder()) yield();
  }

  EVERY_N_SECONDS(1) {
    Serial.printf("Sends: %d | Good:  %d | Bad: %d\n", goodSends + badSends, goodSends, badSends);
    Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
    goodSends = 0;
    badSends = 0;
    Serial.printf("Wifi channel: %d\n", WiFi.channel());
  }

  for (int i = 1; i < 8; i++) {
    // value is sin function with 10 second period scaled to 0-255
    int period = 2000;
    u8 v = (sin((millis() / (float)period) * 2 * PI) + 1) * 127.5;

    int hue = (i * 32) % 256;
    CRGB color = CHSV(hue, 255, v);
    fill_solid(ledMessage.payloadUpdateLeds.leds, NUM_LEDS, color);
    ledMessage.header.deviceId = i;

    handleSendUpdate();
    delay(10);
  }

  // static uint8_t hue = 0;
  // for (int i = 0; i < NUM_LEDS; i++) {
  //   ledMessage.payloadUpdateLeds.leds[i] = CHSV(hue++, 255, 255);
  //   handleSendUpdate();

  //   fadeall();
  //   delay(10);
  // }

  // for (int i = (NUM_LEDS)-1; i >= 0; i--) {
  //   ledMessage.payloadUpdateLeds.leds[i] = CHSV(hue++, 255, 255);
  //   handleSendUpdate();

  //   fadeall();
  //   delay(10);
  // }
}