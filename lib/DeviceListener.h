class DeviceListener {
 public:
  virtual auto DevicePlugged(const std::string& device_id) -> void = 0;
  virtual auto DeviceUnplugged(const std::string& device_id) -> void = 0;
};