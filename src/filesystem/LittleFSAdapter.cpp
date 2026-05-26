#include "filesystem/LittleFSAdapter.h"
#include <LittleFS.h>

bool LittleFSAdapter::begin()
{
    return LittleFS.begin(true);
}

bool LittleFSAdapter::readFile(const char* path, std::string& out) const
{
    std::string full_path = std::string("/") + path;
    File file = LittleFS.open(full_path.c_str(), "r");
    if (!file)
        return false;
    out.clear();
    while (file.available())
        out += static_cast<char>(file.read());
    file.close();
    return true;
}
