#pragma once
#include <string_view>
#include "../path_provider/path_provider.h"
#include "../persistent_storage/persistent_storage.h"

class DatabaseCommandHandler
{
public:
    explicit DatabaseCommandHandler(const PathProvider &pathProvider, const PersistentStorage &persistentStorage) : pathProvider(pathProvider), persistentStorage(persistentStorage) {}
    void createNewDatabase(std::string_view name) const;
    void createNewTable(std::string_view name) const;
    void dropDatabase(std::string_view name) const;
    void dropTable(std::string_view name) const;

private:
    const PathProvider &pathProvider;
    const PersistentStorage &persistentStorage;
};