#pragma once

#include <string>
#include <vector>

#include "logging/Logger.h"

class MockLogger : public Logger
{
    public:
        std::vector<std::string> entries;

        void println(const char* message) override
        {
            entries.emplace_back(message != nullptr ? message : "");
        }

        void print(const char* message) override
        {
            entries.emplace_back(message != nullptr ? message : "");
        }

        void print(int value) override
        {
            entries.emplace_back(std::to_string(value));
        }

        void print(unsigned long value) override
        {
            entries.emplace_back(std::to_string(value));
        }

        void print(float value) override
        {
            entries.emplace_back(std::to_string(value));
        }

        bool contains(const std::string& text) const
        {
            for (const auto& entry : entries)
            {
                if (entry.find(text) != std::string::npos)
                {
                    return true;
                }
            }
            return false;
        }
};
