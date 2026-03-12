#include "serialization/ObservationSerializer.h"

#include <cstdio>

const char* ObservationSerializer::toJson(const Observation& observation)
{
    static char payload[256];

    const int written = std::snprintf(
        payload,
        sizeof(payload),
        "{\"station_id\":\"%s\","
        "\"sequence_number\":%lu,"
        "\"timestamp_utc\":%lu,"
        "\"temperature_c\":%.2f,"
        "\"humidity_pct\":%.2f,"
        "\"pressure_hpa\":%.2f}",
        observation.station_id != nullptr ? observation.station_id : "",
        static_cast<unsigned long>(observation.sequence_number),
        static_cast<unsigned long>(observation.timestamp_utc),
        static_cast<double>(observation.temperature_c),
        static_cast<double>(observation.humidity_pct),
        static_cast<double>(observation.pressure_hpa));

    if (written < 0 || written >= static_cast<int>(sizeof(payload)))
    {
        payload[0] = '{';
        payload[1] = '}';
        payload[2] = '\0';
    }

    return payload;
}
