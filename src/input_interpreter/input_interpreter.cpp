#include "input_interpreter.h"
#include <iostream>
#include "../input_constants.h"
#include "../database_command_handler/database_command_handler.h"

void InputInterpreter::interpret(std::string input) const
{
    std::istringstream stringsStream(input);
    std::string command;
    stringsStream >> command;
    if (command == InputConstants::Action::PUT)
    {
        handlePutCommand(stringsStream);
    }
    else if (command == InputConstants::Action::DELETE)
    {
        interpretDeleteCommand(stringsStream);
    }
    else if (command == InputConstants::Action::GET)
    {
        interpretGetCommand(stringsStream);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}

void InputInterpreter::interpretGetCommand(std::istringstream &stringsStream) const
{
    std::string key;
    stringsStream >> key;
    std::cout << "Handling GET command" << std::endl;
    std::cout << "Key: " << key << std::endl;
}

void InputInterpreter::handlePutCommand(std::istringstream &stringStream) const
{
    std::string key;
    stringStream >> key;

    std::string value;
    stringStream >> value;

    std::cout << "Handling PUT command" << std::endl;
    std::cout << "Key: " << key << ", Value: " << value << std::endl;
}

void InputInterpreter::interpretDeleteCommand(std::istringstream &stringStream) const
{
    std::string key;
    stringStream >> key;

    std::cout << "Handling DELETE command" << std::endl;
    std::cout << "Key: " << key << std::endl;
}