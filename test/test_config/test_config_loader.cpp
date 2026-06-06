#include <string>
#include <unordered_map>

#include "doctest.h"
#include "config/ConfigLoader.h"
#include "filesystem/IFileSystem.h"

class StringFileSystem : public IFileSystem
{
    std::unordered_map<std::string, std::string> files_;
public:
    void add(const char* path, std::string content)
    {
        files_[path] = std::move(content);
    }

    bool readFile(const char* path, std::string& out) const override
    {
        auto it = files_.find(path);
        if (it == files_.end())
            return false;
        out = it->second;
        return true;
    }
};

static const char* CONFIG_JSON = R"({
    "station_id": "station-001",
    "sample_interval_ms": 10000,
    "publish_interval_ms": 10000,
    "led_pin": 4,
    "battery_adc_pin": 35,
    "mqtt": {
        "broker_host": "192.168.0.50",
        "broker_port": 1883,
"topic": "weather/station-001/telemetry"
    },
    "sensors": [
        {
            "type": "SHT41",
            "id": "sht41_main",
            "enabled": true,
            "sda_pin": 21,
            "scl_pin": 22,
            "i2c_address": 68
        },
        {
            "type": "BMP280",
            "id": "bmp280_main",
            "enabled": true,
            "sda_pin": 21,
            "scl_pin": 22,
            "i2c_address": 118
        }
    ]
})";

static const char* SECRETS_JSON = R"({
    "wifi_ssid": "TestNetwork",
    "wifi_password": "test_password"
})";

TEST_CASE("ConfigLoader returns station config from JSON")
{
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.station_id == "station-001");
    REQUIRE(config.station.sensors.size() == 2);
    CHECK(config.station.sensors[0].type == SensorType::SHT41);
    CHECK(config.station.sensors[1].type == SensorType::BMP280);
}

TEST_CASE("ConfigLoader returns correct sample and publish intervals")
{
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.sample_interval_ms == 10000);
    CHECK(config.station.publish_interval_ms == 10000);
}

TEST_CASE("ConfigLoader returns led_pin from JSON")
{
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.led_pin == 4);
}

TEST_CASE("ConfigLoader led_pin overrides firmware default of 2")
{
    // Regression: initializeLEDs() must use the config pin, not the hardcoded
    // default (2). This test ensures the loaded pin differs from 2 so that
    // any breakage in led_pin parsing produces a wrong-pin failure.
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.led_pin != 2);
    CHECK(config.station.led_pin == 4);
}

TEST_CASE("ConfigLoader returns battery_adc_pin from JSON")
{
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.battery_adc_pin == 35);
}

TEST_CASE("ConfigLoader uses default battery_adc_pin when not specified")
{
    static const char* CONFIG_NO_ADC = R"({
        "station_id": "station-001",
        "sample_interval_ms": 10000,
        "publish_interval_ms": 10000,
        "mqtt": {
            "broker_host": "192.168.0.50",
            "broker_port": 1883,
            "topic": "weather/station-001/telemetry"
        },
        "sensors": []
    })";

    StringFileSystem fs;
    fs.add("config.json", CONFIG_NO_ADC);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.battery_adc_pin == 34);
}

TEST_CASE("ConfigLoader uses default led_pin when not specified")
{
    static const char* CONFIG_NO_LED = R"({
        "station_id": "station-001",
        "sample_interval_ms": 10000,
        "publish_interval_ms": 10000,
        "mqtt": {
            "broker_host": "192.168.0.50",
            "broker_port": 1883,
            "topic": "weather/station-001/telemetry"
        },
        "sensors": []
    })";

    StringFileSystem fs;
    fs.add("config.json", CONFIG_NO_LED);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.led_pin == 2);
}

TEST_CASE("ConfigLoader returns sensor configuration details")
{
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    REQUIRE(config.station.sensors.size() == 2);

    CHECK(config.station.sensors[0].id == "sht41_main");
    CHECK(config.station.sensors[0].sda_pin == 21);
    CHECK(config.station.sensors[0].scl_pin == 22);
    CHECK(config.station.sensors[0].i2c_address == 0x44);
    CHECK(config.station.sensors[0].enabled == true);

    CHECK(config.station.sensors[1].id == "bmp280_main");
    CHECK(config.station.sensors[1].sda_pin == 21);
    CHECK(config.station.sensors[1].scl_pin == 22);
    CHECK(config.station.sensors[1].i2c_address == 0x76);
    CHECK(config.station.sensors[1].enabled == true);
}

TEST_CASE("ConfigLoader returns network and MQTT config from JSON")
{
    StringFileSystem fs;
    fs.add("config.json", CONFIG_JSON);
    fs.add("secrets.json", SECRETS_JSON);

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.network.WIFI_SSID == "TestNetwork");
    CHECK(config.network.WIFI_PASSWORD == "test_password");
    CHECK(config.mqtt.BROKER_HOST == "192.168.0.50");
    CHECK(config.mqtt.BROKER_PORT == 1883);
    CHECK(config.mqtt.CLIENT_ID == config.station.station_id);
    CHECK(config.mqtt.TOPIC == "weather/station-001/telemetry");
}

TEST_CASE("ConfigLoader uses struct defaults when files are missing")
{
    StringFileSystem fs;

    ConfigLoader loader(fs);
    AppConfig config = loader.load();

    CHECK(config.station.station_id == "default_station");
    CHECK(config.station.sensors.size() == 0);
    CHECK(config.network.WIFI_SSID == "CHANGE_ME");
    CHECK(config.mqtt.BROKER_HOST == "CHANGE_ME");
}
