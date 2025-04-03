#pragma once
#include "sql_interpreter/sql_interpreter.h"

class App
{
public:
    explicit App(const SQLInterpreter &sqlInterpreter) : sqlInterpreter_(sqlInterpreter) {}
    void run() const;

private:
    const SQLInterpreter &sqlInterpreter_;
};