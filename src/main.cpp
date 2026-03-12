#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <vector>

#include "controller/WeatherStationController.h"
#include "config/ConfigLoader.h"
#include "sensors/SensorFactory.h"
#include "config/Secrets.h"
#include "publisher/HttpPublisher.h"
#include "publisher/adapters/WiFiNetworkStatus.h"
#include "publisher/adapters/ArduinoHttpClientAdapter.h"
#include "logging/SerialLogger.h"

void initializeLEDs();
void connectWifi();
void syncTime();
bool initializeController();

constexpr int BLUE_LED_PIN = 2;
constexpr int RED_LED_PIN = 4;
constexpr int SERIAL_BAUD_RATE = 115200;
constexpr int BOOT_DELAY_MS = 200;
constexpr int TICK_INTERVAL_MS = 2000;

WeatherStationController* controller = nullptr;

void setup()
{
    initializeLEDs();

    Serial.begin(SERIAL_BAUD_RATE);
    delay(BOOT_DELAY_MS);

    connectWifi();
    syncTime();

    if (!initializeController())
    {
        while (true)
        {
            digitalWrite(RED_LED_PIN, HIGH);
            delay(300);
            digitalWrite(RED_LED_PIN, LOW);
            delay(300);
        }
    }
}

void loop()
{
    static int failure_count = 0;

    if (controller == nullptr || !controller->tick())
    {
        failure_count++;
        Serial.print("Tick failure count: ");
        Serial.println(failure_count);

        delay(2000);
        return;
    }

    failure_count = 0;
    delay(TICK_INTERVAL_MS);
}

void initializeLEDs()
{
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(BLUE_LED_PIN, OUTPUT);

    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(BLUE_LED_PIN, LOW);
}

void connectWifi()
{
    WiFi.begin(NETWORK_CONFIG.WIFI_SSID, NETWORK_CONFIG.WIFI_PASSWORD);

    Serial.print("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi connected");
}

void syncTime()
{
    const char* ntpServer = "pool.ntp.org";
    const long gmtOffset_sec = 0;
    const int daylightOffset_sec = 0;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    Serial.print("Waiting for NTP time");

    time_t now = time(nullptr);

    while (now < 100000)
    {
        delay(500);
        Serial.print(".");
        now = time(nullptr);
    }

    Serial.println();
    Serial.println("Time synchronized");
}

bool initializeController()
{
    Serial.println("Booting weather station...");

    ConfigLoader config_loader;
    StationConfig station_config = config_loader.load();

    SensorFactory sensor_factory;
    static std::vector<Sensor*> sensors = sensor_factory.createSensors(station_config);
    
    static WiFiNetworkStatus network_status;
    static ArduinoHttpClientAdapter http_client;
    static SerialLogger logger;

    static HttpPublisher publisher(HTTP_CONFIG.HTTP_ENDPOINT, &network_status, &http_client, &logger);
    static std::vector<Publisher*> publishers{&publisher}; // For now we only have one publisher, but we can easily extend this to support multiple publishers in the future

    controller = new WeatherStationController(
        station_config.station_id,
        std::move(sensors),
        publishers
    );

    if (!controller->initialize())
    {
        Serial.println("Controller failed to initialize.");
        return false;
    }

    Serial.println("Controller initialized successfully.");
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(BLUE_LED_PIN, HIGH);
    return true;
}
