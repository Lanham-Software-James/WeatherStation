#include "serialization/ObservationSerializer.h"

#include <cstdio>
#include <cstring>

namespace
{
    void formatUtcIso8601(std::time_t timestamp, char* buffer, std::size_t size)
    {
        if (buffer == nullptr || size == 0)
        {
            return;
        }

        std::tm time_info{};
#if defined(_WIN32)
        gmtime_s(&time_info, &timestamp);
#else
        gmtime_r(&timestamp, &time_info);
#endif
        std::strftime(buffer, size, "%Y-%m-%dT%H:%M:%SZ", &time_info);
    }
}

const char* ObservationSerializer::toJson(const ObservationBatch& batch)
{
    static char payload[2048];
    payload[0] = '\0';

    char sent_at_buffer[32]{};
    formatUtcIso8601(batch.sent_at, sent_at_buffer, sizeof(sent_at_buffer));

    int written = std::snprintf(
        payload,
        sizeof(payload),
        "{\"station_id\":\"%s\",\"sent_at\":\"%s\",\"samples\":[",
        batch.station_id != nullptr ? batch.station_id : "",
        sent_at_buffer);

    if (written < 0 || written >= static_cast<int>(sizeof(payload)))
    {
        std::strcpy(payload, "{}");
        return payload;
    }

    std::size_t offset = static_cast<std::size_t>(written);

    for (std::size_t i = 0; i < batch.samples.size(); ++i)
    {
        const Observation& sample = batch.samples[i];

        char ts_buffer[32]{};
        formatUtcIso8601(sample.timestamp_utc, ts_buffer, sizeof(ts_buffer));

        written = std::snprintf(
            payload + offset,
            sizeof(payload) - offset,
            "%s{\"ts\":\"%s\",\"temperature_c\":%.2f,\"humidity_pct\":%.2f,\"pressure_hpa\":%.2f}",
            (i == 0 ? "" : ","),
            ts_buffer,
            static_cast<double>(sample.temperature_c),
            static_cast<double>(sample.humidity_pct),
            static_cast<double>(sample.pressure_hpa));

        if (written < 0 || written >= static_cast<int>(sizeof(payload) - offset))
        {
            std::strcpy(payload, "{}");
            return payload;
        }

        offset += static_cast<std::size_t>(written);
    }

    written = std::snprintf(payload + offset, sizeof(payload) - offset, "]}");

    if (written < 0 || written >= static_cast<int>(sizeof(payload) - offset))
    {
        std::strcpy(payload, "{}");
        return payload;
    }

    return payload;
}
