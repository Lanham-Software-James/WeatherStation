#include "config/ConfigLoader.h"
#include <ArduinoJson.h>
#include <cstdlib>
#include <cstring>

static SensorType parseSensorType(const char* type_str)
{
    if (type_str && std::strcmp(type_str, "BMP280") == 0)
        return SensorType::BMP280;
    return SensorType::SHT41;
}

ConfigLoader::ConfigLoader(IFileSystem& fs) : fs_(fs) {}

AppConfig ConfigLoader::load() const
{
    AppConfig config{};

    std::string config_json;
    if (fs_.readFile("config.json", config_json))
    {
        JsonDocument doc;
        if (!deserializeJson(doc, config_json))
        {
            if (const char* id = doc["station_id"].as<const char*>())
                config.station.station_id = id;
            if (!doc["sample_interval_ms"].isNull())
                config.station.sample_interval_ms = doc["sample_interval_ms"].as<unsigned long>();
            if (!doc["publish_interval_ms"].isNull())
                config.station.publish_interval_ms = doc["publish_interval_ms"].as<unsigned long>();
            if (!doc["led_pin"].isNull())
                config.station.led_pin = doc["led_pin"].as<int>();

            JsonObject mqtt_obj = doc["mqtt"].as<JsonObject>();
            if (!mqtt_obj.isNull())
            {
                if (const char* host = mqtt_obj["broker_host"].as<const char*>())
                    config.mqtt.BROKER_HOST = host;
                if (!mqtt_obj["broker_port"].isNull())
                    config.mqtt.BROKER_PORT = mqtt_obj["broker_port"].as<int>();
                if (const char* topic = mqtt_obj["topic"].as<const char*>())
                    config.mqtt.TOPIC = topic;
            }

            JsonArray sensors_arr = doc["sensors"].as<JsonArray>();
            for (JsonObject s : sensors_arr)
            {
                SensorConfig sc{};
                sc.type = parseSensorType(s["type"].as<const char*>());
                if (const char* sid = s["id"].as<const char*>())
                    sc.id = sid;
                sc.enabled = s["enabled"] | true;
                sc.sda_pin = s["sda_pin"] | 21;
                sc.scl_pin = s["scl_pin"] | 22;

                auto addr = s["i2c_address"];
                if (addr.is<int>())
                    sc.i2c_address = addr.as<std::uint8_t>();
                else if (const char* addr_str = addr.as<const char*>())
                    sc.i2c_address = static_cast<std::uint8_t>(std::strtoul(addr_str, nullptr, 0));

                sc.temperature_offset_c = s["temperature_offset_c"] | 0.0f;
                sc.humidity_offset_pct = s["humidity_offset_pct"] | 0.0f;
                sc.pressure_offset_hpa = s["pressure_offset_hpa"] | 0.0f;

                config.station.sensors.push_back(sc);
            }
        }
    }

    config.mqtt.CLIENT_ID = config.station.station_id;

    std::string secrets_json;
    if (fs_.readFile("secrets.json", secrets_json))
    {
        JsonDocument doc;
        if (!deserializeJson(doc, secrets_json))
        {
            if (const char* ssid = doc["wifi_ssid"].as<const char*>())
                config.network.WIFI_SSID = ssid;
            if (const char* pass = doc["wifi_password"].as<const char*>())
                config.network.WIFI_PASSWORD = pass;
        }
    }

    return config;
}
