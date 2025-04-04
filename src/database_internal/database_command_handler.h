#pragma once
#include <string_view>
#include "../path_provider/path_provider.h"

class DatabaseCommandHandler
{
public:
    explicit DatabaseCommandHandler(const PathProvider &pathProvider) : pathProvider(pathProvider) {}
    void createNewDatabase(std::string_view name) const;
    void createNewTable(std::string_view name) const;

private:
    const PathProvider &pathProvider;
};