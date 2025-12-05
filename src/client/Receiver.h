#include <ESP8266WiFi.h>
#include <espnow.h>

#include "NonVolatile.h"
#include "protocol.h"

#define BUFFER_SIZE 4

// Circular buffer for incoming messages
// static struct {
//   uint8_t messageData[sizeof(Message)];
//   uint8_t length;
// } messagesBuffer[BUFFER_SIZE] = {};

// static uint8_t bufferFront = 0;
// static uint8_t bufferBack = 0;

static Message messageBuffer = {0};
static uint8_t messageLength = 0;
static bool messageAvailable = false;

static uint16_t order = 0;

class Receiver {
  private:
      static void pushMessage(const Message* msg, const uint8_t length) {
        memcpy(&messageBuffer, msg, length);
        messageLength = length;
        messageAvailable = true;
      }

      // static void onRecv(uint8_t* mac, uint8_t* incomingData, uint8_t length) {
      //   // Reject messages from other WMNDLS networks
      //   uint32_t id;
      //   if (length < sizeof(uint32_t)) return;
      //   memcpy(&id, incomingData, sizeof(uint32_t));
      //   if (id != NETWORK_ID) return;
      //   pushMessage((Message*)incomingData, length);
      // }

      static void onRecv(uint8_t* mac, uint8_t* incomingData, uint8_t length) {
        // Reject messages from other WMNDLS networks
        uint32_t id;
        if (length < sizeof(uint32_t)) return;

        memcpy(&id, incomingData, sizeof(uint32_t));
        if (id != NETWORK_ID) return;

        pushMessage((Message*)incomingData, length);
      }

      static bool shouldProcceed() {
        Message* message = &messageBuffer;
        // Synchronize message order
        if (message->header.type == MessageType::ORDER) {
          order = message->header.order;
          return false;
        }

        // Reject old messages
        if (message->header.order <= order) return false;

        // Check destination channels
        if (message->header.channels == 0) return false;  // No channel

        // Check broadcast
        if (message->header.channels == BROADCAST_CHANNEL) return true;

        // Check specific channel
        for (uint8_t id = 0; id < 8; id++) {
          if ((message->header.channels & (1 << id)) == storage.channelId) return true;
        }

        return false;
      }

  public:
      static void setup() {
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != 0) return;
        esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
        esp_now_register_recv_cb(onRecv);
      }

      static const bool hasNewMessages() {
        if (!shouldProcceed()) popMessage();  // Discard unwanted message
        return messageAvailable;
      }

      /** Removes the oldest message from the buffer, if any */
      static void popMessage() {
        messageAvailable = false;
      }

      /** Returns a pointer to the oldest message in the buffer */
      static Message* getMessage() {
        return &messageBuffer;
      }

      /** Returns a pointer to the length of the oldest message in the buffer */
      static uint8_t* getMessageLength() {
        return (uint8_t*)&messageLength;
      }
};