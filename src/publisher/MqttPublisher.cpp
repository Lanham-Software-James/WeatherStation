#include "publisher/MqttPublisher.h"
#include "serialization/ObservationSerializer.h"

MqttPublisher::MqttPublisher(
    const char* broker_host,
    int broker_port,
    const char* client_id,
    const char* topic,
    MqttClient* mqtt_client,
    Logger* logger)
    : broker_host_(broker_host),
      broker_port_(broker_port),
      client_id_(client_id),
      topic_(topic),
      mqtt_client_(mqtt_client),
      logger_(logger)
{
}

const char* MqttPublisher::getName() const
{
    return "MqttPublisher";
}

bool MqttPublisher::onInitialize()
{
    if (mqtt_client_ == nullptr)
    {
        if (logger_ != nullptr) logger_->println("MqttPublisher init failed: mqtt_client is null");
        return false;
    }

    mqtt_client_->setServer(broker_host_, broker_port_);
    mqtt_client_->setBufferSize(2048);
    return reconnect();
}

bool MqttPublisher::onPublish(const ObservationBatch& batch)
{
    mqtt_client_->loop();

    if (!mqtt_client_->connected() && !reconnect())
    {
        return false;
    }

    const char* payload = ObservationSerializer::toJson(batch);
    const bool ok = mqtt_client_->publish(topic_, payload);

    if (!ok && logger_ != nullptr)
        logger_->println("MqttPublisher: publish failed");

    return ok;
}

bool MqttPublisher::reconnect()
{
    if (mqtt_client_->connect(client_id_))
    {
        if (logger_ != nullptr) logger_->println("MQTT connected.");
        return true;
    }

    if (logger_ != nullptr)
    {
        logger_->print("MQTT connect failed, state=");
        logger_->print(mqtt_client_->state());
        logger_->println("");
    }
    return false;
}
