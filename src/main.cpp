#include <iostream>
#include <string>
#include "sql_interpreter/sql_interpreter.h"
#include <boost/di.hpp>
#include "app.h"
#include "database_internal/database_command_handler.h"

using namespace std;

int main()
{
    DatabaseCommandHandler databaseCommandHandler;
    SQLInterpreter interpreter(databaseCommandHandler);
    App app(interpreter);
    app.run();
    return 0;
}