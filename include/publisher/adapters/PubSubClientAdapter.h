#pragma once

#include <PubSubClient.h>
#include <WiFiClient.h>

#include "publisher/network/MqttClient.h"

class PubSubClientAdapter : public MqttClient
{
public:
    PubSubClientAdapter();

    void setServer(const char* host, int port) override;
    void setBufferSize(int size) override;
    bool connect(const char* client_id) override;
    bool publish(const char* topic, const char* payload) override;
    bool connected() override;
    bool loop() override;
    int state() override;

private:
    WiFiClient wifi_client_;
    PubSubClient client_;
};
