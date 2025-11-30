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

class CState {
  private:
  Ticker ticker;
  TState current = TState::STATE_BOOT;
  bool changed = true;

  public:
  inline const TState getCurrent() {
    return current;
  }

  inline bool hasChanged() {
    if (changed) {
      changed = false;
      return true;
    } else
      return false;
  }

  void
  set(TState newState) {
    changed = true;
    current = newState;
  }
} State;