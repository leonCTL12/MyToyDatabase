#include <iostream>
#include <string>
#include "SQLInterpreter/SQLInterpreter.h"
#include "DatabaseInternal/test.h"

using namespace std;

int main()
{
    string userInput;
    SQLInterpreter interpreter;
    Test test;

    test.testFunction(); // Call the test function

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