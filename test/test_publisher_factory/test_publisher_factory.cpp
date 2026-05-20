#include "doctest.h"

#include <string>

#include "config/StationConfig.h"
#include "publisher/PublisherFactory.h"
#include "../test/mocks/MockConstructedPublisher.h"

TEST_CASE("PublisherFactory creates enabled publishers only")
{
    PublisherFactory factory(
        [](const PublisherConfig& config) { return new MockConstructedPublisher("HttpPublisher", config.id); }
    );

    StationConfig config{};
    config.publishers = {
        {PublisherType::HTTP, "http_main", true, ""}
    };

    auto publishers = factory.createPublishers(config);

    REQUIRE(publishers.size() == 1);
    CHECK(std::string(publishers[0]->getName()) == "HttpPublisher");

    for (auto* p : publishers) delete p;
}

TEST_CASE("PublisherFactory skips disabled publishers")
{
    PublisherFactory factory(
        [](const PublisherConfig& config) { return new MockConstructedPublisher("HttpPublisher", config.id); }
    );

    StationConfig config{};
    config.publishers = {
        {PublisherType::HTTP, "a", false, ""},
        {PublisherType::HTTP, "b", true, ""},
        {PublisherType::HTTP, "c", false, ""}
    };

    auto publishers = factory.createPublishers(config);

    REQUIRE(publishers.size() == 1);

    auto* mock = static_cast<MockConstructedPublisher*>(publishers[0]);
    CHECK(std::string(mock->getId()) == "b");

    for (auto* p : publishers) delete p;
}

TEST_CASE("PublisherFactory preserves publisher order")
{
    PublisherFactory factory(
        [](const PublisherConfig& config) { return new MockConstructedPublisher("HttpPublisher", config.id); }
    );

    StationConfig config{};
    config.publishers = {
        {PublisherType::HTTP, "first", true, ""},
        {PublisherType::HTTP, "second", true, ""}
    };

    auto publishers = factory.createPublishers(config);

    REQUIRE(publishers.size() == 2);

    auto* first = static_cast<MockConstructedPublisher*>(publishers[0]);
    auto* second = static_cast<MockConstructedPublisher*>(publishers[1]);
    CHECK(std::string(first->getId()) == "first");
    CHECK(std::string(second->getId()) == "second");

    for (auto* p : publishers) delete p;
}

TEST_CASE("PublisherFactory handles missing creator")
{
    PublisherFactory factory(nullptr);

    StationConfig config{};
    config.publishers = {
        {PublisherType::HTTP, "http_main", true, ""}
    };

    auto publishers = factory.createPublishers(config);

    REQUIRE(publishers.size() == 0);
}

TEST_CASE("PublisherFactory passes config to creator")
{
    PublisherFactory factory(
        [](const PublisherConfig& config) { return new MockConstructedPublisher("HttpPublisher", config.id); }
    );

    StationConfig config{};
    config.publishers = {
        {PublisherType::HTTP, "alpha", true, ""},
        {PublisherType::HTTP, "beta", true, ""}
    };

    auto publishers = factory.createPublishers(config);

    REQUIRE(publishers.size() == 2);

    auto* first = static_cast<MockConstructedPublisher*>(publishers[0]);
    auto* second = static_cast<MockConstructedPublisher*>(publishers[1]);
    CHECK(std::string(first->getId()) == "alpha");
    CHECK(std::string(second->getId()) == "beta");

    for (auto* p : publishers) delete p;
}

TEST_CASE("PublisherFactory returns empty vector when no publishers configured")
{
    PublisherFactory factory(
        [](const PublisherConfig& config) { return new MockConstructedPublisher("HttpPublisher", config.id); }
    );

    StationConfig config{};

    auto publishers = factory.createPublishers(config);

    REQUIRE(publishers.size() == 0);
}
