#pragma once

class HttpClient
{
    public:
        virtual ~HttpClient() = default;

        virtual bool begin(const char* url) = 0;
        virtual void addHeader(const char* name, const char* value) = 0;
        virtual int post(const char* payload) = 0;
        virtual void end() = 0;
};
