#pragma once

#include "filesystem/IFileSystem.h"

class LittleFSAdapter : public IFileSystem
{
public:
    bool begin();
    bool readFile(const char* path, std::string& out) const override;
};
