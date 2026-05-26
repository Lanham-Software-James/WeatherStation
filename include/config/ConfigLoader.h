#pragma once

#include "config/AppConfig.h"
#include "filesystem/IFileSystem.h"

class ConfigLoader
{
public:
    explicit ConfigLoader(IFileSystem& fs);
    [[nodiscard]] AppConfig load() const;

private:
    IFileSystem& fs_;
};
