#pragma once
#include "sql_interpreter/sql_interpreter.h"

class App
{
public:
    explicit App(const ISQLInterpreter &sqlInterpreter) : sqlInterpreter_(sqlInterpreter) {}
    void run() const;

private:
    const ISQLInterpreter &sqlInterpreter_;
};