#pragma once

#include "publisher/network/HttpClient.h"

class MockHttpClient : public HttpClient
{
public:
    bool begin_result{true};
    int post_result{200};

    bool begin_called{false};
    bool add_header_called{false};
    bool post_called{false};
    bool end_called{false};

    const char* last_url{nullptr};
    const char* last_header_name;
    const char* last_header_value;
    const char* last_payload;

    bool begin(const char* url) override
    {
        begin_called = true;
        last_url = url;
        return begin_result;
    }

    void addHeader(const char* name, const char* value) override
    {
        add_header_called = true;
        last_header_name = name;
        last_header_value = value;
    }

    int post(const char* payload) override
    {
        post_called = true;
        last_payload = payload;
        return post_result;
    }

    void end() override
    {
        end_called = true;
    }
};
