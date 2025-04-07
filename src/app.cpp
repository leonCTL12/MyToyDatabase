#include "app.h"
#include "input_constants.h"
#include <iostream>

void App::run() const
{
    std::string userInput;

    while (true)
    {
        std::cout << "Enter a command or 'EXIT' to exit: " << std::endl;
        std::cout << ">>> ";
        std::getline(std::cin, userInput);

        if (userInput == InputConstants::Action::EXIT)
        {
            std::cout << "Bye!\n";
            break;
        }

        inputInterpreter_.interpret(userInput);
    }
}