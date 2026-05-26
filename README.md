# Weather Station

ESP32-based weather station firmware — the data collection layer of a hyper-localized weather prediction system. Multiple units are deployed across nearby locations to gather environmental data that feeds an ML pipeline for local weather forecasting.

## Overview

Each station reads temperature, humidity, and barometric pressure at regular intervals and publishes batches of observations over HTTP to a central aggregation endpoint. Several stations distributed across a geographic area form the sensor network; the collected data is used to train models that predict weather conditions specific to that locale.

This repository contains only the firmware. The aggregation backend and ML training pipeline are separate components of the broader system.

## Hardware

**Microcontroller**: ESP32 development board

**Sensors** (I2C):
| Sensor | Measurements | Default Address |
|--------|-------------|-----------------|
| Sensirion SHT41 | Temperature, Humidity | 0x44 |
| Adafruit BMP280 | Barometric Pressure | 0x76 |

**Indicator LEDs**:
| Pin | Color | Meaning |
|-----|-------|---------|
| 2 | Blue | Ready / operating normally |
| 4 | Red | Error / fault condition |

**I2C Pins**: SDA → GPIO 21, SCL → GPIO 22

## Getting Started

### Prerequisites

- [PlatformIO](https://platformio.org/) (CLI or VS Code extension)
- ESP32 development board with SHT41 and/or BMP280 sensors wired via I2C

### Configuration

Before building, create `include/config/Secrets.h` with your network credentials and endpoint:

```cpp
#pragma once

// WiFi
const char* WIFI_SSID     = "your-ssid";
const char* WIFI_PASSWORD = "your-password";

// Data aggregation endpoint
const char* HTTP_ENDPOINT = "http://your-aggregator/observations";
```

This file is excluded from version control (`.gitignore`).

### Build and Flash

```bash
# Build firmware for ESP32
pio run -e esp32dev

# Build, flash, and open serial monitor in one step
pio run -e esp32dev --target upload && pio device monitor -e esp32dev

# Serial monitor only (115200 baud)
pio device monitor -e esp32dev
```

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
HTTP Publisher ──► Aggregation Endpoint
```

The controller samples all enabled sensors every **10 seconds**, accumulating `Observation` records in memory. Every **60 seconds** it serializes the buffered batch to JSON and POSTs it to the configured endpoint. This batching strategy reduces network overhead while preserving per-sample timestamps.

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
| `Publisher` | Abstract base; subclasses implement delivery (currently HTTP) |
| `SensorFactory` / `PublisherFactory` | Create component instances via injected factory functions for testability |
| `ObservationSerializer` | Converts `ObservationBatch` structs to JSON |
| `ConfigLoader` | Loads station ID, intervals, sensor configuration, and per-sensor calibration offsets |
| `Logger` / `Clock` | Platform abstractions that allow hardware-free unit testing |

The codebase applies the **Template Method** pattern for sensor and publisher lifecycles, and uses **dependency injection** via factory function pointers to decouple component creation from component use — both patterns are critical for the native-platform test suite.

### Failure Handling

The controller tracks consecutive failures. After **5 consecutive sampling or publishing failures**, the controller resets itself. The red LED lights whenever the station is in an error state.

## Sensor Calibration

Each sensor entry in the configuration supports measurement offsets for field calibration:

```cpp
SensorConfig{
    .type            = SensorType::SHT41,
    .sda_pin         = 21,
    .scl_pin         = 22,
    .i2c_address     = 0x44,
    .enabled         = true,
    .temp_offset     = -0.5f,   // degrees C
    .humidity_offset = 1.2f,    // percent RH
    .pressure_offset = 0.0f,    // hPa
}
```

## Project Structure

```
weather-station/
├── src/
│   ├── main.cpp                    # ESP32 entry point (WiFi, NTP, main loop)
│   ├── controller/                 # WeatherStationController
│   ├── sensors/
│   │   ├── Sensor.cpp              # Abstract base
│   │   └── adapters/               # SHT41, BMP280 drivers
│   ├── publisher/
│   │   ├── Publisher.cpp           # Abstract base
│   │   └── adapters/               # HttpPublisher
│   ├── config/                     # ConfigLoader, config structs
│   ├── serialization/              # ObservationSerializer
│   ├── logging/                    # Logger abstraction + SerialLogger
│   └── time/                       # Clock abstraction + ArduinoClock
├── include/                        # Headers (mirrors src/ structure)
├── test/
│   ├── mocks/                      # Mock implementations for all interfaces
│   └── test_*/                     # Unit test suites (doctest)
├── test_endpoint/                  # Local HTTP endpoint for manual testing
└── platformio.ini                  # Build configuration (esp32dev + native)
```

## CI

GitHub Actions runs the native test suite on every push and pull request to `main`. See [`.github/workflows/testing.yml`](.github/workflows/testing.yml).

## Dependencies

| Library | Purpose |
|---------|---------|
| [Adafruit BMP280](https://github.com/adafruit/Adafruit_BMP280_Library) | BMP280 pressure sensor driver |
| [Sensirion I2C SHT4x](https://github.com/Sensirion/arduino-i2c-sht4x) | SHT41 temperature/humidity driver |
| [doctest](https://github.com/doctest/doctest) | Header-only unit testing (native tests only) |

## Related Components

This firmware is one part of a larger hyper-localized weather prediction system:

- **Weather Station** ← you are here — sensor firmware for ESP32
- **Aggregation Backend** — collects observation batches from all stations
- **ML Pipeline** — trains forecasting models on the aggregated multi-station dataset
