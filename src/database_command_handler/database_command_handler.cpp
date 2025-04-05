#include "database_command_handler.h"
#include <iostream>
#include <string_view>

void DatabaseCommandHandler::createNewDatabase(std::string_view name) const
{
    std::cout << "Creating new database: " << name << std::endl;
    fs::path dbPath = pathProvider.getDBFolderPath(name);
    if (persistentStorage.tryCreateFolder(dbPath))
    {
        std::cout << "Database folder created successfully in: " << dbPath << std::endl;
    }
    else
    {
        std::cout << "Failed to create database folder or it already exists." << std::endl;
    }
}

void DatabaseCommandHandler::createNewTable(std::string_view name) const
{
    std::cout << "Creating new table: " << name << std::endl;
}