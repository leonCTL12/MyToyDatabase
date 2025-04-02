#include "SQLInterpreter.h"
#include <iostream>
#include "SQLConstants.h"
#include "../DatabaseInternal/DatabaseCommandHandler.h"

void SQLInterpreter::Interpret(std::string input)
{
    std::istringstream stringsStream(input);
    std::string command;
    stringsStream >> command;
    if (command == SQLConstants::Action::CREATE)
    {
        InterpretCreateCommand(stringsStream);
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}

void SQLInterpreter::InterpretCreateCommand(std::istringstream &stringStream)
{
    DatabaseCommandHandler DatabaseCommandHandler;

    std::string objectType;
    stringStream >> objectType;

    std::string name;
    stringStream >> name;

    if (objectType == SQLConstants::ObjectType::DATABASE)
    {
        DatabaseCommandHandler.CreateNewDatabase(name);
    }
    else if (objectType == SQLConstants::ObjectType::TABLE)
    {
        DatabaseCommandHandler.CreateNewTable(name);
    }
    else
    {
        std::cerr << "Unknown object type: " << objectType << std::endl;
    }
}