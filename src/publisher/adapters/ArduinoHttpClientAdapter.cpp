#include "publisher/adapters/ArduinoHttpClientAdapter.h"

bool ArduinoHttpClientAdapter::begin(const char* url)
{
    return http_.begin(url);
}

void ArduinoHttpClientAdapter::addHeader(const char* name, const char* value)
{
    http_.addHeader(name, value);
}

int ArduinoHttpClientAdapter::post(const char* payload)
{
    return http_.POST(payload);
}

void ArduinoHttpClientAdapter::end()
{
    http_.end();
}
