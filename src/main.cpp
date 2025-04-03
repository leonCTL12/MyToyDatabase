#include <iostream>
#include <string>
#include "SQLInterpreter/SQLInterpreter.h"
#include <boost/di.hpp>
#include "DatabaseInternal/DatabaseCommandHandler.h"

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

        interpreter.Interpret(userInput);
    }

    return 0;
}