#include <Arduino.h>

#include "controller/WeatherStationController.h"

WeatherStationController::WeatherStationController(std::string station_id, std::vector<Sensor*> sensors)
    : station_id_(station_id), sensors_(std::move(sensors))
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

void WeatherStationController::tick()
{
    Observation obs;

    for (Sensor* sensor : sensors_)
    {
        sensor->read(obs);
    }
    Serial.print("Station ID: ");
    Serial.print(station_id_.c_str());

    Serial.print("\t\tTemp: ");
    Serial.print(obs.temperature_c);

    Serial.print("\tHumidity: ");
    Serial.print(obs.humidity_pct);

    Serial.print("\t\tPressure: ");
    Serial.println(obs.pressure_hpa);
}
