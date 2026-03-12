#include "publisher/HttpPublisher.h"
#include "serialization/ObservationSerializer.h"

HttpPublisher::HttpPublisher(
    const char* endpoint_url,
    NetworkStatus* network_status,
    HttpClient* http_client,
    Logger* logger)
    : endpoint_url_(endpoint_url),
      network_status_(network_status),
      http_client_(http_client),
      logger_(logger)
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
        if (logger_ != nullptr) logger_->println("HttpPublisher init failed: endpoint is null");
        return false;
    }

    if (network_status_ == nullptr)
    {
        if (logger_ != nullptr) logger_->println("HttpPublisher init failed: network status is null");
        return false;
    }

    if (http_client_ == nullptr)
    {
        if (logger_ != nullptr) logger_->println("HttpPublisher init failed: HTTP client is null");
        return false;
    }

    if (logger_ == nullptr)
    {
        return false;
    }

    if (!network_status_->isConnected())
    {
        if (logger_ != nullptr) logger_->println("HttpPublisher init failed: network not connected");
        return false;
    }

    return true;
}

bool HttpPublisher::onPublish(const ObservationBatch& batch)
{
    if (!network_status_->isConnected())
    {
        if (logger_ != nullptr) logger_->println("HttpPublisher publish failed: network disconnected");
        return false;
    }

    if (!http_client_->begin(endpoint_url_))
    {
        if (logger_ != nullptr) logger_->println("HttpPublisher failed to begin HTTP connection");
        return false;
    }

    http_client_->addHeader("Content-Type", "application/json");

    const char* payload = ObservationSerializer::toJson(batch);
    const int response_code = http_client_->post(payload);

     http_client_->end();

    if (response_code <= 0)
    {
        if (logger_ != nullptr)
        {
            logger_->print("HTTP POST failed: ");
            logger_->print(response_code);
            logger_->println("");
        }
        return false;
    }

    if (logger_ != nullptr)
    {
        logger_->print("HTTP response: ");
        logger_->print(response_code);
        logger_->println("");
    }

    return response_code >= 200 && response_code < 300;
}
