#pragma once

#include <sstream>
#include <string>
#include "../database_command_handler/database_command_handler.h"

class SQLInterpreter
{
public:
    explicit SQLInterpreter(const DatabaseCommandHandler &databaseCommandHandler) : databaseCommandHandler_(databaseCommandHandler) {}
    void interpret(std::string command) const;

private:
    void interpretCreateCommand(std::istringstream &command) const;
    const DatabaseCommandHandler &databaseCommandHandler_;
};