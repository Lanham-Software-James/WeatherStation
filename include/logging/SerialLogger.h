#pragma once

#include "logging/Logger.h"

class SerialLogger : public Logger
{
    public:
        void println(const char* message) override;
        void print(const char* message) override;
        void print(int value) override;
        void print(unsigned long value) override;
        void print(float value) override;
};
