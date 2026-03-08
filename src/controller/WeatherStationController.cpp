#include <Arduino.h>

#include "controller/WeatherStationController.h"

WeatherStationController::WeatherStationController(Sensor* sensor)
    : sensor_(sensor)
{
}

bool WeatherStationController::initialize()
{
    if (sensor_ == nullptr)
    {
        Serial.println("Controller init failed: sensor is null.");
        return false;
    }

    const bool success = sensor_->initialize();

    if (success)
    {
        Serial.println("Controller initialized successfully.");
    }
    else
    {
        Serial.println("Controller init failed: sensor initialization failed.");
    }

    return success;
}

bool WeatherStationController::tick()
{
    if (sensor_ == nullptr)
    {
        Serial.println("Tick failed: sensor is null.");
        return false;
    }

    Observation observation{};

    const bool success = sensor_->read(observation);

    if (!success)
    {
        Serial.println("Tick failed: sensor read failed.");
        return false;
    }

    Serial.print("Temperature: ");
    Serial.print(observation.temperature_c);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(observation.humidity_pct);
    Serial.println(" %");

    return true;
}
