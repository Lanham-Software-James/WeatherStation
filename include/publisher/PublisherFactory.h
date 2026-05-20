#pragma once

#include <functional>
#include <vector>

#include "config/StationConfig.h"
#include "publisher/Publisher.h"

class PublisherFactory
{
public:
    using PublisherCreator = std::function<Publisher*(const PublisherConfig&)>;

    explicit PublisherFactory(PublisherCreator http_creator);

    std::vector<Publisher*> createPublishers(const StationConfig& config);

private:
    PublisherCreator http_creator_;
};
