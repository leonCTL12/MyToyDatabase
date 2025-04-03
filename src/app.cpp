#include "app.h"
#include <iostream>

void App::run() const
{
    std::string userInput;

    while (true)
    {
        std::cout << "Enter a command or 'quit' to exit: " << std::endl;
        std::cout << ">>> ";
        std::getline(std::cin, userInput);

        if (userInput == "quit")
        {
            std::cout << "Bye!\n";
            break;
        }

        sqlInterpreter_.interpret(userInput);
    }
}