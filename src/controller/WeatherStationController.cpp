#include <Arduino.h>
#include <ctime>

#include "controller/WeatherStationController.h"

WeatherStationController::WeatherStationController(std::string station_id, std::vector<Sensor*> sensors)
    : station_id_(std::move(station_id)), sensors_(std::move(sensors))
{
}

bool WeatherStationController::initialize()
{
    if (sensors_.empty())
    {
        Serial.println("Controller init failed: no sensors configured.");
        return false;
    }

    for (Sensor* sensor : sensors_)
    {
        if (sensor == nullptr)
        {
            Serial.println("Controller init failed: null sensor.");
            return false;
        }

        Serial.print("Initializing sensor: ");
        Serial.println(sensor->getName().c_str());

        if (!sensor->initialize())
        {
            Serial.print("Failed to initialize sensor: ");
            Serial.println(sensor->getName().c_str());
            return false;
        }
    }

    Serial.println("Controller initialized successfully.");
    return true;
}

bool WeatherStationController::tick()
{
    Observation obs{};
    obs.station_id = station_id_;
    obs.sequence_number = sequence_number_++;
    obs.timestamp_utc = std::time(nullptr);

    for (Sensor* sensor : sensors_)
    {
        if (!sensor->read(obs))
        {
            Serial.print("Failed to read sensor: ");
            Serial.println(sensor->getName().c_str());
            return false;
        }
    }

    Serial.print("Station ID: ");
    Serial.println(obs.station_id.c_str());

    Serial.print("Sequence Number: ");
    Serial.println(obs.sequence_number);

    Serial.print("Temperature: ");
    Serial.println(obs.temperature_c);

    Serial.print("Humidity: ");
    Serial.println(obs.humidity_pct);

    Serial.print("Pressure: ");
    Serial.println(obs.pressure_hpa);

    Serial.print("Timestamp (UTC): ");
    Serial.println(obs.timestamp_utc);

    Serial.println("-----------------------------");

    return true;
}
