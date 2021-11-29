#ifndef DEVICE_LISTENER_H
#define DEVICE_LISTENER_H

#include <iostream>
class DeviceListener {
 public:
  std::string device_id;
  virtual auto DevicePlugged(const std::string& device_id) -> void = 0;
  virtual auto DeviceUnplugged(const std::string& device_id) -> void = 0;
};

#endif  // DEVICE_LISTENER_H