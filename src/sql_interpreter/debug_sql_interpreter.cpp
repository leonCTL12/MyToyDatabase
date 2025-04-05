#include "debug_sql_interpreter.h"
#include <iostream>

void DebugSQLInterpreter::interpret(std::string command) const
{
    std::cout << "DebugSQLInterpreter: Interpreting command: " << command << std::endl;

    if (command == "RESET")
    {
        persistentStorage_.tryDeleteFolder(pathProvider_.getAppRootPath());
        return;
    }
    sqlInterpreter_.interpret(command);
}