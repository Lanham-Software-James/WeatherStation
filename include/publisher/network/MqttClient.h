#pragma once

class MqttClient
{
public:
    virtual ~MqttClient() = default;
    virtual void setServer(const char* host, int port) = 0;
    virtual void setBufferSize(int size) = 0;
    virtual bool connect(const char* client_id) = 0;
    virtual bool publish(const char* topic, const char* payload) = 0;
    virtual bool connected() = 0;
    virtual bool loop() = 0;
    virtual int state() = 0;
};
