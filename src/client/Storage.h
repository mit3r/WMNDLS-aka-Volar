#include <EEPROM.h>

#pragma once

struct structStorage {
  uint8_t channelId = 0;  // between 0 and 7
};

template <typename T>
class Storage {
  public:
  T* data = new T();

  Storage() {
    EEPROM.begin(sizeof(T));
    EEPROM.get<T>(0, *(this->data));
  };

  ~Storage() {
    EEPROM.end();
  };

  void save() {
    EEPROM.put<T>(0, *(this->data));
    EEPROM.commit();
  };
};

Storage<structStorage> storage;