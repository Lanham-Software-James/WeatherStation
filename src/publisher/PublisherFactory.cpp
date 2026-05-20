#include "publisher/PublisherFactory.h"

PublisherFactory::PublisherFactory(PublisherCreator http_creator)
    : http_creator_(std::move(http_creator))
{
}

std::vector<Publisher*> PublisherFactory::createPublishers(const StationConfig& config)
{
    std::vector<Publisher*> publishers;

    for (const auto& publisher_config : config.publishers)
    {
        if (!publisher_config.enabled)
        {
            continue;
        }

        switch (publisher_config.type)
        {
            case PublisherType::HTTP:
                if (http_creator_)
                {
                    publishers.push_back(http_creator_(publisher_config));
                }
                break;
        }
    }

    return publishers;
}
