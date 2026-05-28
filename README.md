# Weather Station

ESP32-based weather station firmware — the data collection layer of a hyper-localized weather prediction system. Multiple units are deployed across nearby locations to gather environmental data that feeds an ML pipeline for local weather forecasting.

## Overview

Each station reads temperature, humidity, and barometric pressure at regular intervals and publishes batches of observations over MQTT to a broker. Several stations distributed across a geographic area form the sensor network; the collected data is used to train models that predict weather conditions specific to that locale.

This repository contains only the firmware. The aggregation backend and ML training pipeline are separate components of the broader system.

## Hardware

**Microcontroller**: ESP32 development board

**Sensors** (I2C):
| Sensor | Measurements | Default Address |
|--------|-------------|-----------------|
| Sensirion SHT41 | Temperature, Humidity | 0x44 |
| Adafruit BMP280 | Barometric Pressure | 0x76 |

**Indicator LED**:
| Pin | Color |
|-----|-------|
| 2   | Green |

**I2C Pins**: SCL → GPIO 21, SDA → GPIO 22

## LED Status Patterns

| Pattern | Meaning |
|---------|---------|
| Slow blink (500 ms on/off) | Booting |
| Fast blink (150 ms on/off) | Connecting to Wi-Fi |
| Brief pulse (100 ms) | Successful publish |
| Double blink | Recoverable fault — Wi-Fi dropped or controller being reset after repeated failures |
| Solid on | Unrecoverable error — controller failed to initialize; device will keep retrying |

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- ESP32 development board with SHT41 and/or BMP280 sensors wired via I2C

### Configuration

Configuration is stored in two JSON files on the ESP32's LittleFS filesystem. Copy the examples and fill in your values:

```bash
cp data/config.json.example data/config.json
cp data/secrets.json.example data/secrets.json
```

**`data/config.json`** — non-secret station settings:

```json
{
    "station_id": "station_001",
    "sample_interval_ms": 10000,
    "publish_interval_ms": 60000,
    "mqtt": {
        "broker_host": "your-broker-host",
        "broker_port": 1883,
        "topic": "weather/station_001/telemetry"
    },
    "sensors": [
        {
            "type": "SHT41",
            "id": "sht41_main",
            "enabled": true,
            "sda_pin": 21,
            "scl_pin": 22,
            "i2c_address": 68,
            "temperature_offset_c": 0.0,
            "humidity_offset_pct": 0.0
        },
        {
            "type": "BMP280",
            "id": "bmp280_main",
            "enabled": true,
            "sda_pin": 21,
            "scl_pin": 22,
            "i2c_address": 118,
            "pressure_offset_hpa": 0.0
        }
    ]
}
```

**`data/secrets.json`** — WiFi credentials:

```json
{
    "wifi_ssid": "your-ssid",
    "wifi_password": "your-password"
}
```

Both files are excluded from version control (`.gitignore`). The `*.example` files are safe to commit and serve as templates.

### Build and Flash

```bash
# Build firmware for ESP32
pio run -e esp32dev

# Flash config files to LittleFS (required on first flash or after config changes)
pio run -e esp32dev --target uploadfs

# Flash firmware
pio run -e esp32dev --target upload

# Flash filesystem + firmware, then open serial monitor
pio run -e esp32dev --target uploadfs && pio run -e esp32dev --target upload && pio device monitor -e esp32dev

# Serial monitor only (115200 baud)
pio device monitor -e esp32dev
```

> **Note:** `uploadfs` must be run at least once to provision `config.json` and `secrets.json` onto the device. After that, only re-run it when you change those files.

### Run Tests

The test suite runs on the native platform (no hardware required):

```bash
pio test -e native
```

## Architecture

### Data Flow

```
Sensors (10s interval)
   │
   ▼
Observation Buffer
   │
   ▼ (every 60s)
MQTT Publisher ──► MQTT Broker
```

The controller samples all enabled sensors every **10 seconds** (configurable via `sample_interval_ms`), accumulating `Observation` records in memory. Every **60 seconds** (configurable via `publish_interval_ms`) it serializes the buffered batch to JSON and publishes it to the configured MQTT topic. This batching strategy reduces network overhead while preserving per-sample timestamps.

### Observation Payload

```json
{
  "station_id": "station_001",
  "sent_at": "2023-11-14T16:01:00Z",
  "samples": [
    {
      "ts": "2023-11-14T16:00:00Z",
      "temperature_c": 21.52,
      "humidity_pct": 48.10,
      "pressure_hpa": 1012.32
    }
  ]
}
```

All timestamps are UTC ISO 8601, synchronized at boot via NTP (`pool.ntp.org`).

### Key Components

| Component | Description |
|-----------|-------------|
| `WeatherStationController` | Central orchestrator — manages sampling/publishing intervals, observation buffer, and failure recovery |
| `Sensor` | Abstract base; subclasses implement hardware-specific I2C reads |
| `Publisher` | Abstract base; subclasses implement delivery (currently MQTT via `MqttPublisher`) |
| `SensorFactory` / `PublisherFactory` | Create component instances via injected factory functions for testability |
| `ObservationSerializer` | Converts `ObservationBatch` structs to JSON |
| `ConfigLoader` | Reads `config.json` and `secrets.json` from LittleFS; populates station ID, intervals, MQTT settings, sensor configuration, and per-sensor calibration offsets |
| `IFileSystem` / `LittleFSAdapter` / `NativeFileAdapter` | Filesystem abstraction; LittleFS for device, native file I/O for tests |
| `Logger` / `Clock` | Platform abstractions that allow hardware-free unit testing |

The codebase applies the **Template Method** pattern for sensor and publisher lifecycles, and uses **dependency injection** via factory function pointers to decouple component creation from component use — both patterns are critical for the native-platform test suite.

### Failure Handling

The controller tracks consecutive failures. After **5 consecutive sampling or publishing failures**, the controller resets itself. A **double blink** indicates a recoverable fault (Wi-Fi drop or reset after repeated failures); a **solid-on** LED indicates an unrecoverable initialization error (device keeps retrying).

## Sensor Calibration

Each sensor entry in `config.json` supports measurement offsets for field calibration:

```json
{
    "type": "SHT41",
    "id": "sht41_main",
    "enabled": true,
    "sda_pin": 21,
    "scl_pin": 22,
    "i2c_address": 68,
    "temperature_offset_c": -0.5,
    "humidity_offset_pct": 1.2
}
```

```json
{
    "type": "BMP280",
    "id": "bmp280_main",
    "enabled": true,
    "sda_pin": 21,
    "scl_pin": 22,
    "i2c_address": 118,
    "pressure_offset_hpa": 0.0
}
```

After editing offsets, re-flash the filesystem with `pio run -e esp32dev --target uploadfs`.

## Project Structure

```
weather-station/
├── data/
│   ├── config.json.example         # Station/MQTT/sensor config template
│   └── secrets.json.example        # WiFi credentials template
├── src/
│   ├── main.cpp                    # ESP32 entry point (WiFi, NTP, main loop)
│   ├── controller/                 # WeatherStationController
│   ├── sensors/
│   │   ├── Sensor.cpp              # Abstract base
│   │   └── adapters/               # SHT41, BMP280 drivers
│   ├── publisher/
│   │   ├── Publisher.cpp           # Abstract base
│   │   └── adapters/               # MqttPublisher (PubSubClient adapter)
│   ├── filesystem/                 # IFileSystem, LittleFSAdapter, NativeFileAdapter
│   ├── config/                     # ConfigLoader, config structs
│   ├── serialization/              # ObservationSerializer
│   ├── logging/                    # Logger abstraction + SerialLogger
│   └── time/                       # Clock abstraction + ArduinoClock
├── include/                        # Headers (mirrors src/ structure)
├── test/
│   ├── mocks/                      # Mock implementations for all interfaces
│   └── test_*/                     # Unit test suites (doctest)
└── platformio.ini                  # Build configuration (esp32dev + native)
```

## CI

GitHub Actions runs the native test suite on every push and pull request to `main`. See [`.github/workflows/testing.yml`](.github/workflows/testing.yml).

## Dependencies

| Library | Purpose |
|---------|---------|
| [Adafruit BMP280](https://github.com/adafruit/Adafruit_BMP280_Library) | BMP280 pressure sensor driver |
| [Sensirion I2C SHT4x](https://github.com/Sensirion/arduino-i2c-sht4x) | SHT41 temperature/humidity driver |
| [PubSubClient](https://github.com/knolleary/pubsubclient) | MQTT client for publishing observations |
| [ArduinoJson](https://arduinojson.org/) | JSON parsing for file-based configuration |
| [doctest](https://github.com/doctest/doctest) | Header-only unit testing (native tests only) |

## Related Components

This firmware is one part of a larger hyper-localized weather prediction system:

- **Weather Station** ← you are here — sensor firmware for ESP32
- **[Aggregation Backend](https://github.com/Lanham-Software-James/WeatherStationDataPipeline)** — collects observation batches from all stations
- **ML Pipeline** — trains forecasting models on the aggregated multi-station dataset
