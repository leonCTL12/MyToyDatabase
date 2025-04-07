#pragma once

#include <sstream>
#include <string>

class InputInterpreter
{
public:
    explicit InputInterpreter() {}
    void interpret(std::string command) const;

    void interpretGetCommand(std::istringstream &stringsStream) const;

private:
    void handlePutCommand(std::istringstream &command) const;
    void interpretDeleteCommand(std::istringstream &command) const;
};