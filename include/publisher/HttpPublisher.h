#pragma once

#include "model/Observation.h"
#include "publisher/Publisher.h"
#include "publisher/network/HttpClient.h"
#include "publisher/network/NetworkStatus.h"
#include "logging/Logger.h"

class HttpPublisher : public Publisher
{
public:
    HttpPublisher(
        const char* endpoint_url,
        NetworkStatus* network_status,
        HttpClient* http_client,
        Logger* logger);

    [[nodiscard]] const char* getName() const override;

protected:
    bool onInitialize() override;
    bool onPublish(const ObservationBatch& batch) override;

private:
    const char* endpoint_url_;
    NetworkStatus* network_status_;
    HttpClient* http_client_;
    Logger* logger_;
};
