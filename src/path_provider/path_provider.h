#pragma once
#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

class PathProvider
{

public:
    explicit PathProvider();
    fs::path getAppRootPath() const;
    fs::path getDBFolderPath(std::string_view dbName) const;

private:
    fs::path homeDirectory;
};