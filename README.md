# BL999 Sensor Component for ESPHome

[![ESPHome](https://img.shields.io/badge/ESPHome-2025.10.4+-green.svg)](https://esphome.io/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

ESPHome external component for BL999 temperature and humidity sensors using 433MHz RF protocol.

## Supported Devices

- BL999 Temperature & Humidity Sensor
- Compatible with 433MHz RF receivers

## Features

- ✅ Temperature monitoring (°C)
- ✅ Humidity monitoring (%)
- ✅ Battery status monitoring
- ✅ Channel detection (1-3)
- ✅ Power UUID tracking
- ✅ Checksum validation
- ✅ Efficient signal filtering

## Installation

### Method 1: External Component (Recommended)

Add to your ESPHome configuration:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/dungeon77/bl999-esphome
    components: [bl999_sensor]

remote_receiver:
  pin: 4
  id: bl999_receiver
  tolerance: 25%
  filter: 50us
  idle: 10ms

bl999_sensor:
  receiver_id: bl999_receiver
  temperature:
    name: "Living Room Temperature"
  humidity:
    name: "Living Room Humidity"
  battery:
    name: "Living Room Battery"
  channel:
    name: "Living Room Channel"
  power_uuid:
    name: "Living Room Power UUID"
