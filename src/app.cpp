#include "app.h"
#include <iostream>

void App::run() const
{
    std::string userInput;

    while (true)
    {
        std::cout << "Enter a command or 'EXIT' to exit: " << std::endl;
        std::cout << ">>> ";
        std::getline(std::cin, userInput);

        if (userInput == "EXIT")
        {
            std::cout << "Bye!\n";
            break;
        }

        sqlInterpreter_.interpret(userInput);
    }
}