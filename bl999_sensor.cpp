#include "bl999_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace bl999_sensor {

static const char *const TAG = "bl999_sensor";

void BL999Sensor::setup() {
  // Регистрируемся как слушатель в remote_receiver
  if (this->receiver_ != nullptr) {
    this->receiver_->register_listener(this);
    ESP_LOGD(TAG, "Registered as remote_receiver listener");
  } else {
    ESP_LOGE(TAG, "No remote_receiver found!");
  }
}

void BL999Sensor::dump_config() {
  ESP_LOGCONFIG(TAG, "BL999 Sensor:");
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Humidity", this->humidity_sensor_);
  LOG_SENSOR("  ", "Battery", this->battery_sensor_);
  LOG_SENSOR("  ", "Channel", this->channel_sensor_);
  LOG_SENSOR("  ", "Power UUID", this->power_uuid_sensor_);
}

bool BL999Sensor::on_receive(remote_base::RemoteReceiveData data) {
  ESP_LOGV(TAG, "Received raw data, size: %d", data.size());
  
  auto raw_data = data.get_raw_data();
  return this->parse_bl999_message_(raw_data);
}

bool BL999Sensor::parse_bl999_message_(const std::vector<int32_t> &data) {
  if (data.size() < 5) return false;
  
  uint8_t bl999_data[9] = {0};
  int bit_count = 0;
  bool in_message = false;
  
  for (size_t i = 0; i < data.size(); i++) {
    int pulse_length = abs(data[i]);
    bool is_high = data[i] > 0;
    
    if (!in_message) {
      if (!is_high && pulse_length > 8000 && pulse_length < 10000) {
        in_message = true;
        bit_count = 0;
        memset(bl999_data, 0, sizeof(bl999_data));
      }
      continue;
    }
    
    if (is_high) {
      if (pulse_length < 450 || pulse_length > 650) {
        in_message = false;
        continue;
      }
    } else {
      bool bit_value;
      if (pulse_length > 3400 && pulse_length < 4400) {
        bit_value = true;
      } else if (pulse_length > 1350 && pulse_length < 2350) {
        bit_value = false;
      } else {
        in_message = false;
        continue;
      }
      
      int nibble_index = bit_count / 4;
      int bit_in_nibble = bit_count % 4;
      bl999_data[nibble_index] |= (bit_value << bit_in_nibble);
      bit_count++;
      
      if (bit_count >= 36) {
        in_message = false;
        
        uint8_t checksum = 0;
        for (int j = 0; j < 8; j++) {
          checksum += bl999_data[j];
        }
        checksum &= 0x0F;
        
        if (checksum != bl999_data[8]) {
          ESP_LOGV(TAG, "Checksum mismatch: calc=0x%X, recv=0x%X", checksum, bl999_data[8]);
          return false;
        }
        
        uint8_t channel = ((bl999_data[1] & 0x1) << 1) | ((bl999_data[1] & 0x2) >> 1);
        uint8_t power_uuid = (bl999_data[0] << 2) | ((bl999_data[1] & 0xC) >> 2);
        bool battery_low = bl999_data[2] & 0x1;
        
        int16_t temp_raw = (bl999_data[5] << 8) | (bl999_data[4] << 4) | bl999_data[3];
        float temperature;
        if (bl999_data[5] & 0x8) {
          temp_raw = (temp_raw ^ 0xFFF) + 1;
          temperature = -temp_raw / 10.0f;
        } else {
          temperature = temp_raw / 10.0f;
        }
        
        uint8_t humidity_raw = (bl999_data[7] << 4) | bl999_data[6];
        uint8_t humidity = 100 - humidity_raw;
        
        if (this->temperature_sensor_ != nullptr)
          this->temperature_sensor_->publish_state(temperature);
        if (this->humidity_sensor_ != nullptr)
          this->humidity_sensor_->publish_state(humidity);
        if (this->battery_sensor_ != nullptr)
          this->battery_sensor_->publish_state(battery_low ? 0 : 100);
        if (this->channel_sensor_ != nullptr)
          this->channel_sensor_->publish_state(channel);
        if (this->power_uuid_sensor_ != nullptr)
          this->power_uuid_sensor_->publish_state(power_uuid);
        
        ESP_LOGD(TAG, "BL999: Ch=%d, UUID=%d, Temp=%.1fC, Hum=%d%%, Bat=%s", 
                 channel, power_uuid, temperature, humidity, battery_low ? "LOW" : "OK");
        
        return true;
      }
    }
  }
  
  return false;
}

}  // namespace bl999_sensor
}  // namespace esphome