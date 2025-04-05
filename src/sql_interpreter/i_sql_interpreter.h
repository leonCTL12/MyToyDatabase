#pragma once

#include <sstream>
#include <string>

class ISQLInterpreter
{
public:
    // = 0 mean undefined, foces derived classes to implement this method
    virtual void interpret(std::string command) const = 0;
};