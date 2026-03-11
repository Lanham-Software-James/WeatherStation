#include "publisher/HTTPPublisher.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

HTTPPublisher::HTTPPublisher(const char* endpoint_url)
    : endpoint_url_(endpoint_url)
{
}

const char* HTTPPublisher::getName() const
{
    return "HTTPPublisher";
}

bool HTTPPublisher::onInitialize()
{
    if (endpoint_url_ == nullptr)
    {
        Serial.println("HTTPPublisher init failed: endpoint is null");
        return false;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("HTTPPublisher init failed: WiFi not connected");
        return false;
    }

    return true;
}

bool HTTPPublisher::onPublish(const Observation& observation)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("HTTPPublisher publish failed: WiFi disconnected");
        return false;
    }

    HTTPClient http;

    if (!http.begin(endpoint_url_))
    {
        Serial.println("HTTPPublisher failed to begin HTTP connection");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    String payload = "{";
    payload += "\"station_id\":\"";
    payload += observation.station_id;
    payload += "\",";

    payload += "\"sequence_number\":";
    payload += String(observation.sequence_number);
    payload += ",";

    payload += "\"timestamp_utc\":";
    payload += String((unsigned long)observation.timestamp_utc);
    payload += ",";

    payload += "\"temperature_c\":";
    payload += String(observation.temperature_c, 2);
    payload += ",";

    payload += "\"humidity_pct\":";
    payload += String(observation.humidity_pct, 2);
    payload += ",";

    payload += "\"pressure_hpa\":";
    payload += String(observation.pressure_hpa, 2);

    payload += "}";

    int response_code = http.POST(payload);

    if (response_code <= 0)
    {
        Serial.print("HTTP POST failed: ");
        Serial.println(response_code);
        http.end();
        return false;
    }

    Serial.print("HTTP response: ");
    Serial.println(response_code);

    http.end();

    return response_code >= 200 && response_code < 300;
}
