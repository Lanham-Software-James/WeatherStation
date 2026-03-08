#include <Arduino.h>

#include "controller/WeatherStationController.h"
#include "sensors/SHT41Sensor.h"

SHT41Sensor sht41_sensor;
WeatherStationController controller(&sht41_sensor);

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("Booting weather station...");

    if (!controller.initialize())
    {
        Serial.println("Controller failed to initialize.");
    }
}

void loop()
{
    controller.tick();
    delay(2000);
}
