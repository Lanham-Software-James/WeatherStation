#include "logging/adapters/SerialLogger.h"

#include <Arduino.h>

void SerialLogger::println(const char* message)
{
    Serial.println(message);
}

void SerialLogger::print(const char* message)
{
    Serial.print(message);
}

void SerialLogger::print(int value)
{
    Serial.print(value);
}

void SerialLogger::print(unsigned long value)
{
    Serial.print(value);
}

void SerialLogger::print(float value)
{
    Serial.print(value);
}
