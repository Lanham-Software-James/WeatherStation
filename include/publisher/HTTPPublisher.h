#pragma once

#include "publisher/Publisher.h"

class HTTPPublisher : public Publisher
{
public:
    explicit HTTPPublisher(const char* endpoint_url);

    [[nodiscard]] const char* getName() const override;

protected:
    bool onInitialize() override;
    bool onPublish(const Observation& observation) override;

private:
    const char* endpoint_url_;
};
