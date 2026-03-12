#include <ctime>

#include "controller/WeatherStationController.h"

WeatherStationController::WeatherStationController(
    const char* station_id,
    unsigned long sample_interval_ms,
    unsigned long publish_interval_ms,
    const std::vector<Sensor*>& sensors,
    const std::vector<Publisher*>& publishers,
    Logger* logger,
    Clock* clock)
    : station_id_(station_id),
      sample_interval_ms_(sample_interval_ms),
      publish_interval_ms_(publish_interval_ms),
      sensors_(sensors),
      publishers_(publishers),
      logger_(logger),
      clock_(clock)
{
}

bool WeatherStationController::initialize()
{
    if (logger_ == nullptr)
    {
        return false;
    }
    
    if (station_id_ == nullptr)
    {
        if (logger_ != nullptr) logger_->println("Controller init failed: station_id is null.");
        return false;
    }

    if (sensors_.empty())
    {
        logger_->println("Controller init failed: no sensors provided.");
        return false;
    }

    if (publishers_.empty())
    {
        logger_->println("Controller init failed: no publishers provided.");
        return false;
    }

    if (clock_ == nullptr)
    {
        logger_->println("Controller init failed: clock is null.");
        return false;
    }

    for (Sensor* sensor : sensors_)
    {
        if (sensor == nullptr)
        {
            logger_->println("Controller init failed: null sensor.");
            return false;
        }

        logger_->print("Initializing sensor: ");
        logger_->println(sensor->getName());

        if (!sensor->initialize())
        {
            logger_->print("Failed to initialize sensor: ");
            logger_->println(sensor->getName());
            return false;
        }
    }

    for (Publisher* publisher : publishers_)
    {
        if (publisher == nullptr)
        {
            logger_->println("Controller init failed: null publisher.");
            return false;
        }

        logger_->print("Initializing publisher: ");
        logger_->println(publisher->getName());

        if (!publisher->initialize())
        {
            logger_->print("Failed to initialize publisher: ");
            logger_->println(publisher->getName());
            return false;
        }
    }

    const unsigned long now_ms = clock_->millis();
    last_sample_ms_ = now_ms;
    last_publish_ms_ = now_ms;

    logger_->println("Controller initialized successfully.");
    return true;
}

bool WeatherStationController::tick()
{
    const unsigned long now_ms = clock_->millis();

    while (now_ms - last_sample_ms_ >= sample_interval_ms_)
    {
        last_sample_ms_ += sample_interval_ms_;

        if (!sampleSensors())
        {
            return false;
        }
    }

    while (now_ms - last_publish_ms_ >= publish_interval_ms_)
    {
        last_publish_ms_ += publish_interval_ms_;

        if (!publishBatch())
        {
            return false;
        }
    }

    return true;
}

bool WeatherStationController::sampleSensors()
{
    Observation obs{};
    obs.station_id = station_id_;
    obs.sequence_number = ++sequence_number_;
    obs.timestamp_utc = clock_->now();

    for (Sensor* sensor : sensors_)
    {
        if (!sensor->read(obs))
        {
            logger_->print("Failed to read sensor: ");
            logger_->println(sensor->getName());
            return false;
        }
    }

    buffered_samples_.push_back(obs);

    logger_->print("Sample collected. Count = ");
    logger_->print(static_cast<unsigned long>(buffered_samples_.size()));
    logger_->println("");

    return true;
}

bool WeatherStationController::publishBatch()
{
    if (buffered_samples_.empty())
    {
        logger_->println("No samples collected; skipping publish.");
        return true;
    }

    ObservationBatch batch{};
    batch.station_id = station_id_;
    batch.sent_at = clock_->now();
    batch.samples = buffered_samples_;

    for (Publisher* publisher : publishers_)
    {
        if (!publisher->publish(batch))
        {
            logger_->print("Failed to publish batch with publisher: ");
            logger_->println(publisher->getName());
            return false;
        }
    }

    logger_->println("Batch observation published successfully.");
    logger_->print("Published sample count: ");
    logger_->print(static_cast<unsigned long>(buffered_samples_.size()));
    logger_->println("");
    logger_->println("-----------------------------");

    buffered_samples_.clear();

    return true;
}
