#include <Arduino.h>
class Button {
  private:
  unsigned int pin;
  unsigned long timer;
  unsigned long holdTime;

  unsigned char active = 0;
  // 0 - none
  // 1 - press
  // 2 - long press
  // 3 - long long press

  // bool pressActive = false;
  // bool holdActive = false;
  // bool longPressActive = false;
  // bool longLongPressActive = false;

  public:
  // When button is released
  void (*callbackPress)();
  void (*callbackLongPress)();
  void (*callbackLongLongPress)();

  // While button is being held
  void (*callbackHold)();
  void (*callbackLongHold)();

  Button(unsigned int pin, unsigned long holdTime = 250) : pin(pin), holdTime(holdTime) {}

  void handle() {
    if (digitalRead(pin) == LOW) {
      if (active == 0) {
        timer = millis();
        active = 1;
      }

      if (active == 1 && (millis() - timer) > holdTime) {
        active = 2;
        if (callbackHold) callbackHold();
      }

      if (active == 2 && (millis() - timer) > holdTime * 4) {
        active = 3;
        if (callbackLongHold) callbackLongHold();
      }

    } else {
      if (active == 0) return;
      if (active == 1 && callbackPress) callbackPress();
      if (active == 2 && callbackLongPress) callbackLongPress();
      if (active == 3 && callbackLongLongPress) callbackLongLongPress();
      active = 0;
    }
  }
};