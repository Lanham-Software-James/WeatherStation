#pragma once

#include "publisher/Publisher.h"
#include "publisher/network/MqttClient.h"
#include "logging/Logger.h"

class MqttPublisher : public Publisher
{
public:
    MqttPublisher(
        const char* broker_host,
        int broker_port,
        const char* client_id,
        const char* topic,
        MqttClient* mqtt_client,
        Logger* logger);

    [[nodiscard]] const char* getName() const override;

protected:
    bool onInitialize() override;
    bool onPublish(const ObservationBatch& batch) override;

private:
    bool reconnect();

    const char* broker_host_;
    int broker_port_;
    const char* client_id_;
    const char* topic_;
    MqttClient* mqtt_client_;
    Logger* logger_;
};
