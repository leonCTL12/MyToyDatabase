#pragma once

#include <sstream>
#include <string>
#include "../database_command_handler/database_command_handler.h"

class InputInterpreter
{
public:
    explicit InputInterpreter(const DatabaseCommandHandler &databaseCommandHandler) : databaseCommandHandler_(databaseCommandHandler) {}
    void interpret(std::string command) const;

    void interpretGetCommand(std::istringstream &stringsStream) const;

private:
    void handlePutCommand(std::istringstream &command) const;
    void interpretDeleteCommand(std::istringstream &command) const;
    const DatabaseCommandHandler &databaseCommandHandler_;
};