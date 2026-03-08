#include <Arduino.h>

#include "controller/WeatherStationController.h"
#include "sensors/SHT41Sensor.h"
#include "sensors/BMP280Sensor.h"

SHT41Sensor sht41;
BMP280Sensor bmp280;

WeatherStationController controller({&sht41, &bmp280});

constexpr int BLUE_LED_PIN = 2;
constexpr int RED_LED_PIN = 4;
constexpr int SERIAL_BAUD_RATE = 115200;
constexpr int BOOT_DELAY_MS = 2000;
constexpr int TICK_INTERVAL_MS = 2000;

void setup()
{
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);

    Serial.begin(SERIAL_BAUD_RATE);

    Serial.println("Booting weather station...");
    
    delay(BOOT_DELAY_MS);

    if (!controller.initialize())
    {
        Serial.println("Controller failed to initialize.");
        while (true)
        {
            digitalWrite(RED_LED_PIN, HIGH);
            delay(300);
            digitalWrite(RED_LED_PIN, LOW);
            delay(300);
        }
    } else {
        Serial.println("Controller initialized successfully.");
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(BLUE_LED_PIN, HIGH);  
    }
}

void loop()
{
    controller.tick();
    delay(TICK_INTERVAL_MS);
}
