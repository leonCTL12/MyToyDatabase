#pragma once
#include "input_interpreter.h"
#include "../persistent_storage/persistent_storage.h"
#include "../path_provider/path_provider.h"

class DebugInputInterpreter : public ISQLInterpreter
{
    // This is not a decorator pattern, coz I inject a concrete SQLInterpreter instead of an ISQLInterpreter
    // This is intended, coz it is sufficient in such use case
public:
    explicit DebugInputInterpreter(const SQLInterpreter &sqlInterpreter, const PersistentStorage &persistentStorage, const PathProvider &pathProvider) : sqlInterpreter_(sqlInterpreter), persistentStorage_(persistentStorage), pathProvider_(pathProvider) {}
    void interpret(std::string command) const override;

private:
    const SQLInterpreter &sqlInterpreter_;
    const PersistentStorage &persistentStorage_;
    const PathProvider &pathProvider_;
};