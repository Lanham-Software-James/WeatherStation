#pragma once

#include <string>

#include "publisher/network/MqttClient.h"

class MockMqttClient : public MqttClient
{
public:
    bool connect_result{true};
    bool connected_result{true};
    bool publish_result{true};

    bool set_server_called{false};
    bool set_buffer_size_called{false};
    bool connect_called{false};
    bool publish_called{false};
    bool loop_called{false};

    const char* last_host{nullptr};
    int last_port{0};
    int last_buffer_size{0};
    const char* last_client_id{nullptr};
    std::string last_topic{};
    std::string last_payload{};

    void setServer(const char* host, int port) override
    {
        set_server_called = true;
        last_host = host;
        last_port = port;
    }

    void setBufferSize(int size) override
    {
        set_buffer_size_called = true;
        last_buffer_size = size;
    }

    bool connect(const char* client_id) override
    {
        connect_called = true;
        last_client_id = client_id;
        return connect_result;
    }

    bool publish(const char* topic, const char* payload) override
    {
        publish_called = true;
        last_topic = topic ? topic : "";
        last_payload = payload ? payload : "";
        return publish_result;
    }

    bool connected() override
    {
        return connected_result;
    }

    bool loop() override
    {
        loop_called = true;
        return true;
    }

    int state() override
    {
        return connect_result ? 0 : -2;
    }
};
