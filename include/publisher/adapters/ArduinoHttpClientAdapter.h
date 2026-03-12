#pragma once

#include <Arduino.h>
#include <HTTPClient.h>

#include "publisher/network/HttpClient.h"

class ArduinoHttpClientAdapter : public HttpClient
{
    public:
        bool begin(const char* url) override;
        void addHeader(const char* name, const char* value) override;
        int post(const char* payload) override;
        void end() override;

    private:
        HTTPClient http_;
};
