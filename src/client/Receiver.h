#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Storage.h"
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

      static void onRecv(uint8_t* mac, uint8_t* incomingData, uint8_t length) {
        // Reject messages from other WMNDLS networks
        if (((Message*)incomingData)->header.networkId != NETWORK_ID) return;
        pushMessage((Message*)incomingData, length);
      }

      static bool shouldProcceed() {
        Message* message = &messageBuffer;

        Serial.println("ShouldProcceed called");
        Serial.printf("Network ID: %08X\n", message->header.networkId);
        Serial.printf("Message length: %d\n", message->header.length);
        Serial.printf("Message type: %d\n", message->header.type);
        Serial.printf("Message order: %d\n", message->header.order);
        Serial.printf("Channels: ");

        if (message->header.channels == BROADCAST_CHANNEL) {
          Serial.print("broadcast");
        } else {
          for (uint8_t i = 0; i <= 7; i++)
            if ((message->header.channels & (1 << i))) Serial.printf("%d ", i);
        }
        Serial.println();

        // Check destination channels
        if (message->header.channels == 0) return false;  // No channel

        Serial.println("Has valid channel");

        // Check channel
        if (!(message->header.channels == BROADCAST_CHANNEL || (message->header.channels & (1 << storage.data->channelId)))) return false;

        Serial.println("Passed broadcast/channel check");

        // Synchronize message order
        if (message->header.type == MessageType::ORDER)
          return ((order = message->header.order), false);

        Serial.println("Not an order message");

        // Reject old messages
        if (message->header.order <= order)
          return (order = message->header.order, false);

        Serial.println("Is a new message");

        return true;
      }

  public:
      static void setup() {
        WiFi.mode(WIFI_STA);

        if (esp_now_init() != 0) return;
        esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
        esp_now_register_recv_cb(onRecv);
      }

      static const bool hasNewMessages() {
        if (!messageAvailable) return false;

        if (shouldProcceed()) return true;
        popMessage();
        return false;
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
        return &messageLength;
      }
};