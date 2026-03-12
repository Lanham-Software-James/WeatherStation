#include "publisher/HttpPublisher.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

HttpPublisher::HttpPublisher(const char* endpoint_url)
    : endpoint_url_(endpoint_url)
{
}

const char* HttpPublisher::getName() const
{
    return "HttpPublisher";
}

bool HttpPublisher::onInitialize()
{
    if (endpoint_url_ == nullptr)
    {
        Serial.println("HttpPublisher init failed: endpoint is null");
        return false;
    }

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("HttpPublisher init failed: WiFi not connected");
        return false;
    }

    return true;
}

bool HttpPublisher::onPublish(const Observation& observation)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("HttpPublisher publish failed: WiFi disconnected");
        return false;
    }

    HTTPClient http;

    if (!http.begin(endpoint_url_))
    {
        Serial.println("HttpPublisher failed to begin HTTP connection");
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
