#include "filesystem/NativeFileAdapter.h"
#include <fstream>
#include <sstream>

NativeFileAdapter::NativeFileAdapter(std::string data_dir)
    : data_dir_(std::move(data_dir))
{
}

bool NativeFileAdapter::readFile(const char* path, std::string& out) const
{
    std::string full_path = data_dir_ + "/" + path;
    std::ifstream f(full_path);
    if (!f.is_open())
        return false;
    std::ostringstream ss;
    ss << f.rdbuf();
    out = ss.str();
    return true;
}
