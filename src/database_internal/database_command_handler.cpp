#include "database_command_handler.h"
#include <iostream>
#include <string_view>

void DatabaseCommandHandler::createNewDatabase(std::string_view name) const
{
    std::cout << "Creating new database: " << name << std::endl;
    fs::path dbPath = pathProvider.getDBFolderPath(name);
    std::cout << "Database path: " << dbPath << std::endl;
}

void DatabaseCommandHandler::createNewTable(std::string_view name) const
{
    std::cout << "Creating new table: " << name << std::endl;
}