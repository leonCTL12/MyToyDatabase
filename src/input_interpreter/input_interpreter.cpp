#include "input_interpreter.h"
#include <iostream>
#include "../input_constants.h"

void InputInterpreter::interpret(std::string input) const
{
    std::istringstream stringsStream(input);
    std::string command;
    stringsStream >> command;
    if (command == InputConstants::Action::PUT)
    {
        interpretPutCommand(stringsStream);
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

    auto value = keyValueStore_.get(key);

    if (value)
    {
        std::cout << *value << std::endl;
    }
    else
    {
        std::cout << "Key not found. Key: " << key << std::endl;
    }
}

void InputInterpreter::interpretPutCommand(std::istringstream &stringStream) const
{
    std::string key;
    stringStream >> key;

    std::string value;
    stringStream >> value;

    keyValueStore_.put(key, value);
    std::cout << "Success" << std::endl;
}

void InputInterpreter::interpretDeleteCommand(std::istringstream &stringStream) const
{
    std::string key;
    stringStream >> key;

    if (keyValueStore_.deleteKey(key))
    {
        std::cout << "Success" << std::endl;
    }
    else
    {
        std::cout << "Fail" << std::endl;
    }
}