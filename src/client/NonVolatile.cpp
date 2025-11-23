#include <EEPROM.h>

template <typename T>
class NonVolatile {
  private:
  const size_t EEPROM_SIZE = 64;

  T* loadedData = nullptr;
  T* currentData = nullptr;

  public:
  NonVolatile() {
    EEPROM.begin(EEPROM_SIZE);
    EEPROM.get<T>(0, *loadedData);

    memcpy(currentData, loadedData, sizeof(T));
  };

  void update() {
    if (memcmp(loadedData, currentData, sizeof(T)) != 0) {
      EEPROM.put<T>(0, currentData);
      EEPROM.commit();
    }

    EEPROM.end();
  };

  T* get() {
    return currentData;
  }
};