#include "publisher/adapters/PubSubClientAdapter.h"

PubSubClientAdapter::PubSubClientAdapter()
    : client_(wifi_client_)
{
}

void PubSubClientAdapter::setServer(const char* host, int port)
{
    client_.setServer(host, static_cast<uint16_t>(port));
}

void PubSubClientAdapter::setBufferSize(int size)
{
    client_.setBufferSize(static_cast<uint16_t>(size));
}

bool PubSubClientAdapter::connect(const char* client_id)
{
    return client_.connect(client_id);
}

bool PubSubClientAdapter::publish(const char* topic, const char* payload)
{
    return client_.publish(topic, payload);
}

bool PubSubClientAdapter::connected()
{
    return client_.connected();
}

bool PubSubClientAdapter::loop()
{
    return client_.loop();
}

int PubSubClientAdapter::state()
{
    return client_.state();
}
