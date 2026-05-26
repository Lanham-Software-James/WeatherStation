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
#include "publisher/MqttPublisher.h"
#include "publisher/adapters/PubSubClientAdapter.h"
#include "logging/adapters/SerialLogger.h"
#include "time/adapters/ArduinoClock.h"

void initializeLEDs();
void ledOff();
void ledOn();
void ledSlowBlink(int count);
void ledFastBlinkStep();
void ledPulse();
void ledDoubleBlink();
void ledSolid();
bool connectWifi();
bool syncTime();
void initializeHardware();
bool initializeController();

constexpr int GREEN_LED_PIN = 2;
constexpr int LED_SLOW_MS = 500;
constexpr int LED_FAST_MS = 150;
constexpr int LED_PULSE_MS = 100;
constexpr int LED_BLINK_MS = 100;
constexpr int SERIAL_BAUD_RATE = 115200;
constexpr int ERROR_TICK_DELAY_MS = 2000;
constexpr int NTP_POLL_DELAY_MS = 500;
constexpr unsigned long MAX_WIFI_WAIT_MS = 30000;
constexpr unsigned long MAX_NTP_WAIT_MS = 15000;
constexpr int MAX_TICK_FAILURES = 5;

static SerialLogger logger;
static PubSubClientAdapter mqtt_client;
static ArduinoClock clock_instance;
static StationConfig station_config;
static std::vector<Sensor*> sensors;
static std::vector<Publisher*> publishers;

WeatherStationController* controller = nullptr;
static int prev_publish_count = 0;

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
    else
        prev_publish_count = controller->totalSuccessfulPublishes();
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        logger.println("WiFi disconnected; attempting reconnect...");
        ledDoubleBlink();

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
        prev_publish_count = controller->totalSuccessfulPublishes();
    }

    controller->tick();

    const int cur_publish_count = controller->totalSuccessfulPublishes();
    if (cur_publish_count > prev_publish_count)
    {
        ledPulse();
        prev_publish_count = cur_publish_count;
    }

    const int pub_failures    = controller->consecutivePublishFailures();
    const int sample_failures = controller->consecutiveSampleFailures();

    if (pub_failures >= MAX_TICK_FAILURES)
    {
        logger.print("Consecutive publish failures: ");
        logger.print(pub_failures);
        logger.println("; resetting controller.");
        ledDoubleBlink();
        delete controller;
        controller = nullptr;
        delay(ERROR_TICK_DELAY_MS);
        return;
    }

    if (sample_failures >= MAX_TICK_FAILURES)
    {
        logger.print("Consecutive sample failures: ");
        logger.print(sample_failures);
        logger.println("; resetting controller.");
        ledDoubleBlink();
        delete controller;
        controller = nullptr;
        delay(ERROR_TICK_DELAY_MS);
        return;
    }
}

void initializeLEDs()
{
    pinMode(GREEN_LED_PIN, OUTPUT);
    ledSlowBlink(3);
}

void ledOff()  { digitalWrite(GREEN_LED_PIN, LOW); }
void ledOn()   { digitalWrite(GREEN_LED_PIN, HIGH); }

void ledSlowBlink(int count)
{
    for (int i = 0; i < count; i++)
    {
        ledOn();
        delay(LED_SLOW_MS);
        ledOff();
        delay(LED_SLOW_MS);
    }
}

void ledFastBlinkStep()
{
    ledOn();
    delay(LED_FAST_MS);
    ledOff();
    delay(LED_FAST_MS);
}

void ledPulse()
{
    ledOn();
    delay(LED_PULSE_MS);
    ledOff();
}

void ledDoubleBlink()
{
    for (int i = 0; i < 2; i++)
    {
        ledOn();
        delay(LED_BLINK_MS);
        ledOff();
        delay(LED_BLINK_MS);
    }
}

void ledSolid() { ledOn(); }

bool connectWifi()
{
    WiFi.begin(NETWORK_CONFIG.WIFI_SSID, NETWORK_CONFIG.WIFI_PASSWORD);
    logger.print("Connecting to WiFi");

    const unsigned long start_ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - start_ms >= MAX_WIFI_WAIT_MS)
        {
            ledOff();
            logger.println("");
            logger.println("WiFi connection timed out.");
            return false;
        }
        ledFastBlinkStep();
        logger.print(".");
    }

    ledOff();
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
        delay(NTP_POLL_DELAY_MS);
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

    publishers.push_back(new MqttPublisher(
        MQTT_CONFIG.BROKER_HOST,
        MQTT_CONFIG.BROKER_PORT,
        MQTT_CONFIG.CLIENT_ID,
        MQTT_CONFIG.TOPIC,
        &mqtt_client,
        &logger
    ));
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
        ledSolid();
        return false;
    }

    ledOff();
    return true;
}
