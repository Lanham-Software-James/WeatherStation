#pragma once

class Logger
{
    public:
        virtual ~Logger() = default;

        virtual void println(const char* message) = 0;
        virtual void print(const char* message) = 0;
        virtual void print(int value) = 0;
        virtual void print(unsigned long value) = 0;
        virtual void print(float value) = 0;
};
