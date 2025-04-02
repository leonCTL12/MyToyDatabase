#pragma once

#include <sstream>
#include <string>

class SQLInterpreter
{
public:
    void Interpret(std::string command);

private:
    void InterpretCreateCommand(std::istringstream &command);
};