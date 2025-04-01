#include <iostream>
#include <string>

using namespace std;

int main()
{
    string userInput;

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

        cout << userInput << endl;
    }

    return 0;
}