#include <Arduino.h>
#include <FastLED.h>

#include "Button.h"
#include "NonVolatile.h"
#include "Receiver.h"
#include "State.h"
#include "Strip.h"
#include "protocol.h"

// State programs
#include "states/Program.h"
//
#include "states/AddrProgram.h"
#include "states/BootProgram.h"
#include "states/IdleProgram.h"
#include "states/ReceiveProgram.h"

#define FPS 60
#define FRAME_DELAY_MS (1000 / FPS)

#define BUTTON_PIN 3  // GPIO3 (RX)
// #define BUTTON_PIN 0  // GPIO0 (BOOT)

Button button(BUTTON_PIN);
Program* programs[TState::length]{nullptr};

void setup() {
  // DO NOT USE Serial
  // Serial.begin(115200);

  programs[TState::STATE_IDLE] = new IdleProgram();
  programs[TState::STATE_RECV] = new ReceiveProgram();
  programs[TState::STATE_ADDR] = new AddrProgram();
  programs[TState::STATE_BOOT] = new BootProgram();

  Receiver::setup();
  Strip::setup();

  pinMode(BUTTON_PIN, INPUT);

  button.callbackPress = []() {
    Program* program = programs[State.getCurrent()];
    if (program != nullptr) program->onButtonPress();
  };

  button.callbackLongPress = []() {
    Program* program = programs[State.getCurrent()];
    if (program != nullptr) program->onButtonLongPress();
  };

  button.callbackLongLongPress = []() {
    Program* program = programs[State.getCurrent()];
    if (program != nullptr) program->onButtonLongLongPress();
  };
}

void loop() {
  FastLED.show();

  EVERY_N_MILLIS(1000) {                           // 400 without delay
    Serial.printf("FPS: %d\n", FastLED.getFPS());  // Update FPS counter

    Serial.printf("WiFi Channel: %d\n", WiFi.channel());
  }

  button.handle();

  Program* program = programs[State.getCurrent()];

  if (State.hasChanged()) program->setup();
  if (Receiver::hasNewMessage()) {
    program->onMessage(Receiver::popMessage());
  }
  program->loop();

  yield();
}