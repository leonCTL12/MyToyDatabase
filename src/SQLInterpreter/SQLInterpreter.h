#pragma once

#include <sstream>
#include <string>
#include "../DatabaseInternal/DatabaseCommandHandler.h"

class SQLInterpreter
{
public:
    explicit SQLInterpreter(const DatabaseCommandHandler &databaseCommandHandler) : databaseCommandHandler_(databaseCommandHandler) {}
    void Interpret(std::string command) const;

private:
    void InterpretCreateCommand(std::istringstream &command) const;
    const DatabaseCommandHandler &databaseCommandHandler_;
};