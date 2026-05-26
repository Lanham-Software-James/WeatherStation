#pragma once

#include "filesystem/IFileSystem.h"
#include <string>

class NativeFileAdapter : public IFileSystem
{
public:
    explicit NativeFileAdapter(std::string data_dir = "data");
    bool readFile(const char* path, std::string& out) const override;

private:
    std::string data_dir_;
};
