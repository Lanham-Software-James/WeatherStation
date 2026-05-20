#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <vector>

#include "controller/WeatherStationController.h"
#include "config/ConfigLoader.h"
#include "sensors/SensorFactory.h"
#include "sensors/adapters/SHT41Sensor.h"
#include "sensors/adapters/BMP280Sensor.h"
#include "config/Secrets.h"
#include "publisher/PublisherFactory.h"
#include "publisher/HttpPublisher.h"
#include "publisher/adapters/WiFiNetworkStatus.h"
#include "publisher/adapters/ArduinoHttpClientAdapter.h"
#include "logging/adapters/SerialLogger.h"
#include "time/adapters/ArduinoClock.h"

void initializeLEDs();
bool connectWifi();
bool syncTime();
void initializeHardware();
bool initializeController();

constexpr int BLUE_LED_PIN = 2;
constexpr int RED_LED_PIN = 4;
constexpr int SERIAL_BAUD_RATE = 115200;
constexpr int ERROR_TICK_DELAY_MS = 2000;
constexpr int WAITING_DELAY_MS = 500;
constexpr unsigned long MAX_WIFI_WAIT_MS = 30000;
constexpr unsigned long MAX_NTP_WAIT_MS = 15000;
constexpr int MAX_TICK_FAILURES = 5;

static SerialLogger logger;
static WiFiNetworkStatus network_status;
static ArduinoHttpClientAdapter http_client;
static ArduinoClock clock_instance;
static StationConfig station_config;
static std::vector<Sensor*> sensors;
static std::vector<Publisher*> publishers;

WeatherStationController* controller = nullptr;

void setup()
{
    initializeLEDs();
    Serial.begin(SERIAL_BAUD_RATE);

    if (!connectWifi())
        logger.println("WiFi unavailable at startup; will retry in loop.");
    else if (!syncTime())
        logger.println("NTP sync failed; timestamps may be inaccurate.");

    initializeHardware();

    if (!initializeController())
        logger.println("Controller initialization failed; will retry in loop.");
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        logger.println("WiFi disconnected; attempting reconnect...");
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BLUE_LED_PIN, LOW);

        if (!connectWifi())
        {
            logger.println("WiFi reconnect failed.");
            delay(ERROR_TICK_DELAY_MS);
            return;
        }

        syncTime();
    }

    if (controller == nullptr)
    {
        logger.println("Attempting controller initialization...");
        if (!initializeController())
        {
            logger.println("Controller initialization failed; retrying...");
            delay(ERROR_TICK_DELAY_MS);
            return;
        }
    }

    controller->tick();

    const int pub_failures = controller->consecutivePublishFailures();
    const int sample_failures = controller->consecutiveSampleFailures();

    if (pub_failures >= MAX_TICK_FAILURES)
    {
        logger.print("Consecutive publish failures: ");
        logger.print(pub_failures);
        logger.println("; resetting controller.");
        delete controller;
        controller = nullptr;
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BLUE_LED_PIN, LOW);
        delay(ERROR_TICK_DELAY_MS);
        return;
    }

    if (sample_failures >= MAX_TICK_FAILURES)
    {
        logger.print("Consecutive sample failures: ");
        logger.print(sample_failures);
        logger.println("; resetting controller.");
        delete controller;
        controller = nullptr;
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BLUE_LED_PIN, LOW);
        delay(ERROR_TICK_DELAY_MS);
        return;
    }
}

void initializeLEDs()
{
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);
}

bool connectWifi()
{
    WiFi.begin(NETWORK_CONFIG.WIFI_SSID, NETWORK_CONFIG.WIFI_PASSWORD);
    logger.print("Connecting to WiFi");

    const unsigned long start_ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - start_ms >= MAX_WIFI_WAIT_MS)
        {
            logger.println("");
            logger.println("WiFi connection timed out.");
            return false;
        }
        delay(WAITING_DELAY_MS);
        logger.print(".");
    }

    logger.println("");
    logger.println("WiFi connected.");
    return true;
}

bool syncTime()
{
    configTime(0, 0, "pool.ntp.org");
    logger.print("Waiting for NTP time");

    const unsigned long start_ms = millis();
    time_t now = time(nullptr);
    while (now < 100000)
    {
        if (millis() - start_ms >= MAX_NTP_WAIT_MS)
        {
            logger.println("");
            logger.println("NTP sync timed out.");
            return false;
        }
        delay(WAITING_DELAY_MS);
        logger.print(".");
        now = time(nullptr);
    }

    logger.println("");
    logger.println("Time synchronized.");
    return true;
}

void initializeHardware()
{
    logger.println("Initializing hardware...");

    ConfigLoader config_loader;
    station_config = config_loader.load();

    SensorFactory sensor_factory(
        [](const SensorConfig& config) -> Sensor* { return new SHT41Sensor(config); },
        [](const SensorConfig& config) -> Sensor* { return new BMP280Sensor(config); }
    );
    sensors = sensor_factory.createSensors(station_config);

    PublisherFactory publisher_factory(
        [](const PublisherConfig& config) -> Publisher* {
            return new HttpPublisher(HTTP_CONFIG.HTTP_ENDPOINT, &network_status, &http_client, &logger);
        }
    );
    publishers = publisher_factory.createPublishers(station_config);
}

bool initializeController()
{
    logger.println("Booting weather station...");

    delete controller;
    controller = new WeatherStationController(
        station_config.station_id,
        station_config.sample_interval_ms,
        station_config.publish_interval_ms,
        sensors,
        publishers,
        &logger,
        &clock_instance
    );

    if (!controller->initialize())
    {
        logger.println("Controller failed to initialize.");
        delete controller;
        controller = nullptr;
        return false;
    }

    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, HIGH);
    return true;
}
