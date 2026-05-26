#pragma once

#include <string>

class IFileSystem
{
public:
    virtual ~IFileSystem() = default;
    virtual bool readFile(const char* path, std::string& out) const = 0;
};
