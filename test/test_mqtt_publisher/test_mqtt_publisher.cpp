#include "doctest.h"

#include <string>

#include "model/Observation.h"
#include "model/ObservationBatch.h"
#include "publisher/MqttPublisher.h"
#include "mocks/MockMqttClient.h"
#include "mocks/MockLogger.h"

namespace
{
    Observation makeObservation(
        const char* station_id,
        std::time_t ts,
        float temperature_c,
        float humidity_pct,
        float pressure_hpa)
    {
        Observation observation{};
        observation.station_id = station_id;
        observation.timestamp_utc = ts;
        observation.temperature_c = temperature_c;
        observation.humidity_pct = humidity_pct;
        observation.pressure_hpa = pressure_hpa;
        return observation;
    }

    ObservationBatch makeBatch()
    {
        ObservationBatch batch{};
        batch.station_id = "home_ref";
        batch.sent_at = 1700000060;
        batch.samples.push_back(makeObservation("home_ref", 1700000000, 21.52f, 48.10f, 1012.32f));
        batch.samples.push_back(makeObservation("home_ref", 1700000010, 21.60f, 48.00f, 1012.28f));
        return batch;
    }
}

TEST_CASE("MqttPublisher returns correct name")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    CHECK(std::string(publisher.getName()) == "MqttPublisher");
}

TEST_CASE("MqttPublisher initialize fails when mqtt_client is null")
{
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", nullptr, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(logger.contains("mqtt_client is null"));
}

TEST_CASE("MqttPublisher initialize calls setServer with configured host and port")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    CHECK(mqtt_client.set_server_called == true);
    REQUIRE(mqtt_client.last_host != nullptr);
    CHECK(std::string(mqtt_client.last_host) == "192.168.0.50");
    CHECK(mqtt_client.last_port == 1883);
}

TEST_CASE("MqttPublisher initialize calls setBufferSize with 2048")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    CHECK(mqtt_client.set_buffer_size_called == true);
    CHECK(mqtt_client.last_buffer_size == 2048);
}

TEST_CASE("MqttPublisher initialize calls connect with configured client_id")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "station-000", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    CHECK(mqtt_client.connect_called == true);
    REQUIRE(mqtt_client.last_client_id != nullptr);
    CHECK(std::string(mqtt_client.last_client_id) == "station-000");
}

TEST_CASE("MqttPublisher initialize fails when broker connect fails")
{
    MockMqttClient mqtt_client;
    mqtt_client.connect_result = false;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    CHECK(publisher.initialize() == false);
    CHECK(publisher.isInitialized() == false);
    CHECK(logger.contains("MQTT connect failed"));
}

TEST_CASE("MqttPublisher initialize succeeds when broker connect succeeds")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    CHECK(publisher.initialize() == true);
    CHECK(publisher.isInitialized() == true);
    CHECK(logger.contains("MQTT connected"));
}

TEST_CASE("MqttPublisher publish fails before initialization")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    ObservationBatch batch = makeBatch();

    CHECK(publisher.publish(batch) == false);
    CHECK(mqtt_client.publish_called == false);
}

TEST_CASE("MqttPublisher publish calls loop")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();
    REQUIRE(publisher.publish(batch) == true);

    CHECK(mqtt_client.loop_called == true);
}

TEST_CASE("MqttPublisher publish reconnects when broker disconnects between publishes")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    mqtt_client.connected_result = false;
    mqtt_client.connect_called = false;

    ObservationBatch batch = makeBatch();
    CHECK(publisher.publish(batch) == true);
    CHECK(mqtt_client.connect_called == true);
}

TEST_CASE("MqttPublisher publish fails when disconnected and reconnect fails")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    mqtt_client.connected_result = false;
    mqtt_client.connect_result = false;

    ObservationBatch batch = makeBatch();
    CHECK(publisher.publish(batch) == false);
    CHECK(mqtt_client.publish_called == false);
}

TEST_CASE("MqttPublisher publish sends serialized batch to configured topic")
{
    MockMqttClient mqtt_client;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/observations", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();
    REQUIRE(publisher.publish(batch) == true);

    CHECK(mqtt_client.publish_called == true);
    CHECK(mqtt_client.last_topic == "weather/observations");
    CHECK(mqtt_client.last_payload.find("\"station_id\":\"home_ref\"") != std::string::npos);
    CHECK(mqtt_client.last_payload.find("\"sent_at\":\"") != std::string::npos);
    CHECK(mqtt_client.last_payload.find("\"samples\":[") != std::string::npos);
    CHECK(mqtt_client.last_payload.find("\"temperature_c\":21.52") != std::string::npos);
    CHECK(mqtt_client.last_payload.find("\"humidity_pct\":48.10") != std::string::npos);
    CHECK(mqtt_client.last_payload.find("\"pressure_hpa\":1012.32") != std::string::npos);
}

TEST_CASE("MqttPublisher publish fails when mqtt publish returns false")
{
    MockMqttClient mqtt_client;
    mqtt_client.publish_result = false;
    MockLogger logger;

    MqttPublisher publisher("192.168.0.50", 1883, "weather_station", "weather/obs", &mqtt_client, &logger);

    REQUIRE(publisher.initialize() == true);

    ObservationBatch batch = makeBatch();
    CHECK(publisher.publish(batch) == false);
    CHECK(logger.contains("publish failed"));
}
