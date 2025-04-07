#include "input_interpreter.h"
#include <iostream>
#include "input_constants.h"
#include "../database_command_handler/database_command_handler.h"

void SQLInterpreter::interpret(std::string input) const
{
    std::istringstream stringsStream(input);
    std::string command;
    stringsStream >> command;
    if (command == InputConstants::Action::CREATE)
    {
        interpretCreateCommand(stringsStream);
    }
    else if (command == InputConstants::Action::DROP)
    {
        interpretDropCommand(stringsStream);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}

void SQLInterpreter::interpretCreateCommand(std::istringstream &stringStream) const
{
    std::string objectType;
    stringStream >> objectType;

    std::string name;
    stringStream >> name;

    if (objectType == InputConstants::ObjectType::DATABASE)
    {
        databaseCommandHandler_.createNewDatabase(name);
    }
    else if (objectType == InputConstants::ObjectType::TABLE)
    {
        databaseCommandHandler_.createNewTable(name);
    }
    else
    {
        std::cerr << "Unknown object type: " << objectType << std::endl;
    }
}

void SQLInterpreter::interpretDropCommand(std::istringstream &stringStream) const
{
    std::string objectType;
    stringStream >> objectType;

    std::string name;
    stringStream >> name;

    if (objectType == InputConstants::ObjectType::DATABASE)
    {
        databaseCommandHandler_.dropDatabase(name);
    }
    else if (objectType == InputConstants::ObjectType::TABLE)
    {
        databaseCommandHandler_.dropTable(name);
    }
    else
    {
        std::cerr << "Unknown object type: " << objectType << std::endl;
    }
}