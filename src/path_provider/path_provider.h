#pragma once
#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

class PathProvider
{

public:
    fs::path getAppRootPath() const;
    fs::path getDBFolderPath(std::string_view dbName) const;
};