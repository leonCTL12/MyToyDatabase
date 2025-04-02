#include <iostream>
#include <string>
#include "SQLInterpreter/SQLInterpreter.h"

using namespace std;

int main()
{
    string userInput;
    SQLInterpreter interpreter;

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