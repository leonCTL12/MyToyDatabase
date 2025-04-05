#include "sql_interpreter.h"
#include <iostream>
#include "sql_constants.h"
#include "../database_command_handler/database_command_handler.h"

void SQLInterpreter::interpret(std::string input) const
{
    std::istringstream stringsStream(input);
    std::string command;
    stringsStream >> command;
    if (command == SQLConstants::Action::CREATE)
    {
        interpretCreateCommand(stringsStream);
    }
    else if (command == SQLConstants::Action::DROP)
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

    if (objectType == SQLConstants::ObjectType::DATABASE)
    {
        databaseCommandHandler_.createNewDatabase(name);
    }
    else if (objectType == SQLConstants::ObjectType::TABLE)
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

    if (objectType == SQLConstants::ObjectType::DATABASE)
    {
        databaseCommandHandler_.dropDatabase(name);
    }
    else if (objectType == SQLConstants::ObjectType::TABLE)
    {
        databaseCommandHandler_.dropTable(name);
    }
    else
    {
        std::cerr << "Unknown object type: " << objectType << std::endl;
    }
}