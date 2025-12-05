#include "protocol.h"

class Configuration : Config {
  public:
  static Config::RGBResolution resolution;
};

Config::RGBResolution Configuration::resolution = Config::RGBResolution::RGB24;