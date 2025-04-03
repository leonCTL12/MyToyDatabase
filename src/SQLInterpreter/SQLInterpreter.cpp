#include "SQLInterpreter.h"
#include <iostream>
#include "SQLConstants.h"
#include "../DatabaseInternal/DatabaseCommandHandler.h"

void SQLInterpreter::Interpret(std::string input) const
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

void SQLInterpreter::InterpretCreateCommand(std::istringstream &stringStream) const
{
    std::string objectType;
    stringStream >> objectType;

    std::string name;
    stringStream >> name;

    if (objectType == SQLConstants::ObjectType::DATABASE)
    {
        databaseCommandHandler_.CreateNewDatabase(name);
    }
    else if (objectType == SQLConstants::ObjectType::TABLE)
    {
        databaseCommandHandler_.CreateNewTable(name);
    }
    else
    {
        std::cerr << "Unknown object type: " << objectType << std::endl;
    }
}