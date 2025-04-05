#pragma once

#include <sstream>
#include <string>
#include "../database_command_handler/database_command_handler.h"
#include "i_sql_interpreter.h"

class SQLInterpreter : public ISQLInterpreter
{
public:
    explicit SQLInterpreter(const DatabaseCommandHandler &databaseCommandHandler) : databaseCommandHandler_(databaseCommandHandler) {}
    void interpret(std::string command) const;

private:
    void interpretCreateCommand(std::istringstream &command) const;
    void interpretDropCommand(std::istringstream &command) const;
    const DatabaseCommandHandler &databaseCommandHandler_;
};