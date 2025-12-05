#include <ESP8266WiFi.h>
#include <espnow.h>

#include "Commander.h"
#include "Sender.h"
#include "protocol.h"

#define RUN_ONCE \
  for (static bool __flag = false; !__flag; __flag = true)

void setup() {
  Commander::setup();
  Sender::setup();
}

void loop() {
  RUN_ONCE {
    for (int i = 0; i < 5; i++)
      while (!Sender::sendOrderMessage()) yield();
  };

  Commander::loop();
  Sender::loop();

  if (Commander::hasMessage()) {
    Message message = Commander::popMessage();
    while (!Sender::sendMessage(message)) yield();
  }
}