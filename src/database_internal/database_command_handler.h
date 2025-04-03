#pragma once
#include <string_view>

class DatabaseCommandHandler
{
public:
    void createNewDatabase(std::string_view name) const;
    void createNewTable(std::string_view name) const;
};