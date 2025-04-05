#include "path_provider.h"

fs::path PathProvider::getAppRootPath() const
{
    return homeDirectory / "toydb";
}

fs::path PathProvider::getDBFolderPath(std::string_view dbName) const
{
    return getAppRootPath() / dbName;
}

PathProvider::PathProvider()
{
    const char *homeDir = std::getenv("HOME");
    if (homeDir == nullptr)
    {
        throw std::runtime_error("HOME environment variable not set");
    }

    homeDirectory = fs::path(homeDir);
}