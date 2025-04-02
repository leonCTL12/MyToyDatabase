#pragma once
#include <string_view>

class DatabaseCommandHandler
{
public:
    void CreateNewDatabase(std::string_view name);
    void CreateNewTable(std::string_view name);
};