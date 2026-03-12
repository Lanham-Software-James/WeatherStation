#include <Arduino.h>
#include <ctime>

#include "controller/WeatherStationController.h"

WeatherStationController::WeatherStationController(
    const char* station_id,
    const std::vector<Sensor*>& sensors,
    const std::vector<Publisher*>& publishers)
    : station_id_(station_id),
      sensors_(sensors),
      publishers_(publishers)
{
}

bool WeatherStationController::initialize()
{
    if (station_id_ == nullptr)
    {
        Serial.println("Controller init failed: station_id is null.");
        return false;
    }

    if (sensors_.empty())
    {
        Serial.println("Controller init failed: no sensors provided.");
        return false;
    }

    if (publishers_.empty())
    {
        Serial.println("Controller init failed: no publishers provided.");
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
        Serial.println(sensor->getName());

        if (!sensor->initialize())
        {
            Serial.print("Failed to initialize sensor: ");
            Serial.println(sensor->getName());
            return false;
        }
    }

    for (Publisher* publisher : publishers_)
    {
        if (publisher == nullptr)
        {
            Serial.println("Controller init failed: null publisher.");
            return false;
        }

        Serial.print("Initializing publisher: ");
        Serial.println(publisher->getName());

        if (!publisher->initialize())
        {
            Serial.print("Failed to initialize publisher: ");
            Serial.println(publisher->getName());
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
            Serial.println(sensor->getName());
            return false;
        }
    }

    Serial.println("Observation read successfull.");

    Serial.print("Station ID: ");
    Serial.println(obs.station_id);

    Serial.print("Sequence Number: ");
    Serial.println(obs.sequence_number);

    Serial.print("Temperature: ");
    Serial.println(obs.temperature_c);

    Serial.print("Humidity: ");
    Serial.println(obs.humidity_pct);

    Serial.print("Pressure: ");
    Serial.println(obs.pressure_hpa);

    Serial.print("Timestamp (UTC): ");
    Serial.println(localtime(&obs.timestamp_utc), "%Y-%m-%d %H:%M:%S");

    for (Publisher* publisher : publishers_)
    {
        if (!publisher->publish(obs))
        {
            Serial.print("Failed to publish observation with publisher: ");
            Serial.println(publisher->getName());
            return false;
        }
    }

    Serial.println("Observations published successfully.");
    Serial.println("-----------------------------");

    return true;
}
