#include <ESP8266WiFi.h>
#include <espnow.h>

#include "NonVolatile.h"
#include "protocol.h"

static Message message = {};
static bool newMessage = false;
static uint32_t order = 0;

class Receiver {
  private:
  static void onRecv(uint8_t* mac, uint8_t* incomingData, uint8_t len) {
    memcpy(&message, incomingData, sizeof(message));

    // General message validation
    if (message.header.networkId != NETWORK_ID) return;
    if (message.header.deviceId != storage.deviceId && message.header.deviceId != 0) return;
    // Specific message validation
    if ((message.header.type == MessageType::UPDATE_LEDS ||
         message.header.type == MessageType::SET_ADDR) &&
        message.header.order <= order) return;

    if (message.header.type == MessageType::NEW_ORDER)
      order = message.header.order;

    newMessage = true;
  }

  public:
  static void setup() {
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != 0) return;
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(onRecv);
  }

  static const bool
  hasNewMessage() {
    return newMessage;
  }

  static const Message&
  popMessage() {
    // Serial.printf("Received message of type %d with order %d\n", message.header.type, message.header.order);

    newMessage = false;
    order = message.header.order;
    return message;
  }
};