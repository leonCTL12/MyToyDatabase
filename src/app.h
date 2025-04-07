#pragma once
#include "input_interpreter/input_interpreter.h"

class App
{
public:
    explicit App(const InputInterpreter &inputInterpreter) : inputInterpreter_(inputInterpreter) {}
    void run() const;

private:
    const InputInterpreter &inputInterpreter_;
};