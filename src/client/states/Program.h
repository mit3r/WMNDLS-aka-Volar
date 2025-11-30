
#pragma once

struct Program {
  /** Executes once when the program is started */
  virtual void setup() {};

  /** Executes on every iteration */
  virtual void loop() {};

  /** Executes when a new message is received */
  virtual void onMessage(const Message& message) {};

  /** Executes when the button is pressed */
  virtual void onButtonPress() {};

  /** Executes when the button is long pressed */
  virtual void onButtonLongPress() {};

  /** Executes when the button is long long pressed */
  virtual void onButtonLongLongPress() {};

  /** Executes once when the program is stopped */
  virtual void cleanup() {};

  virtual ~Program() {}
};