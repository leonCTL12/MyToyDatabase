#pragma once

#include <sstream>
#include <string>

class ISQLInterpreter
{
public:
    virtual void interpret(std::string command) const = 0;
};