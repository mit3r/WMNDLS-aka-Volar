#include <Ticker.h>

#pragma once

// Device state
enum TState {
  STATE_BOOT,  // Initializing
  STATE_RECV,  // Receiving and displaying data
  STATE_ADDR,  // Setting device address
  STATE_IDLE,  // Power saving
  length,      // Number of states
};

class State {
  private:
      static Ticker ticker;
      static TState current;
      static bool changed;

  public:
      inline static const TState getCurrent() {
        return current;
      }

      inline static bool hasChanged() {
        if (changed) {
          changed = false;
          return true;
        } else
          return false;
      }

      static void set(TState newState) {
        changed = true;
        current = newState;
      }
};

Ticker State::ticker = Ticker();
TState State::current = TState::STATE_BOOT;
bool State::changed = true;