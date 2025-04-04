#include "path_provider.h"

fs::path PathProvider::getAppRootPath() const
{
    fs::path app_root_path = "/var/lib/toydb";
    return app_root_path;
}

fs::path PathProvider::getDBFolderPath(std::string_view dbName) const
{
    return getAppRootPath() / dbName;
}