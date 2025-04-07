#include "debug_input_interpreter.h"
#include <iostream>

void DebugInputInterpreter::interpret(std::string command) const
{
    std::cout << "DebugInputInterpreter: Interpreting command: " << command << std::endl;

    if (command == "RESET")
    {
        persistentStorage_.tryDeleteFolder(pathProvider_.getAppRootPath());
        return;
    }
    sqlInterpreter_.interpret(command);
}