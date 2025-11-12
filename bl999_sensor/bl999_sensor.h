#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/remote_base/remote_base.h"

namespace esphome {
namespace bl999_sensor {

class BL999Sensor : public Component, public remote_base::RemoteReceiverListener {
 public:
  void set_remote_receiver(remote_base::RemoteReceiverBase *receiver) { this->receiver_ = receiver; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_humidity_sensor(sensor::Sensor *humidity_sensor) { humidity_sensor_ = humidity_sensor; }
  void set_battery_sensor(sensor::Sensor *battery_sensor) { battery_sensor_ = battery_sensor; }
  void set_channel_sensor(sensor::Sensor *channel_sensor) { channel_sensor_ = channel_sensor; }
  void set_power_uuid_sensor(sensor::Sensor *power_uuid_sensor) { power_uuid_sensor_ = power_uuid_sensor; }

  void setup() override;
  void dump_config() override;

 protected:
  bool on_receive(remote_base::RemoteReceiveData data) override;
  bool parse_bl999_message_(const std::vector<int32_t> &data);
  
  remote_base::RemoteReceiverBase *receiver_{nullptr};
  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *humidity_sensor_{nullptr};
  sensor::Sensor *battery_sensor_{nullptr};
  sensor::Sensor *channel_sensor_{nullptr};
  sensor::Sensor *power_uuid_sensor_{nullptr};
};

}  // namespace bl999_sensor
}  // namespace esphome