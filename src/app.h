#pragma once
#include "input_interpreter/input_interpreter.h"

class App
{
public:
    explicit App(const ISQLInterpreter &sqlInterpreter) : sqlInterpreter_(sqlInterpreter) {}
    void run() const;

private:
    const ISQLInterpreter &sqlInterpreter_;
};