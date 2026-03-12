#pragma once

#include <string>
#include <vector>

#include "logging/Logger.h"

class MockLogger : public Logger
{
public:
    std::vector<std::string> messages;

    void println(const char* message) override
    {
        messages.emplace_back(message != nullptr ? message : "");
        messages.emplace_back("\n");
    }

    void print(const char* message) override
    {
        messages.emplace_back(message != nullptr ? message : "");
    }

    void print(int value) override
    {
        messages.emplace_back(std::to_string(value));
    }

    void print(unsigned long value) override
    {
        messages.emplace_back(std::to_string(value));
    }

    void print(float value) override
    {
        messages.emplace_back(std::to_string(value));
    }
};
