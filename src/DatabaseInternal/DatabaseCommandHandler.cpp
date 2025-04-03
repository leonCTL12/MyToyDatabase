#include "DatabaseCommandHandler.h"
#include <iostream>
#include <string_view>

void DatabaseCommandHandler::CreateNewDatabase(std::string_view name) const
{
    std::cout << "Creating new database: " << name << std::endl;
}

void DatabaseCommandHandler::CreateNewTable(std::string_view name) const
{
    std::cout << "Creating new table: " << name << std::endl;
}