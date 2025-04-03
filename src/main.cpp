#include <iostream>
#include <string>
#include "sql_interpreter/sql_interpreter.h"
#include <boost/di.hpp>
#include "database_internal/database_command_handler.h"

using namespace std;

int main()
{
    string userInput;
    DatabaseCommandHandler databaseCommandHandler;
    SQLInterpreter interpreter(databaseCommandHandler);

    while (true)
    {
        cout << "Enter a command or 'quit' to exit: " << endl;
        cout << ">>> ";
        getline(cin, userInput);

        if (userInput == "quit")
        {
            cout << "Bye!\n";
            break;
        }

        interpreter.interpret(userInput);
    }

    return 0;
}