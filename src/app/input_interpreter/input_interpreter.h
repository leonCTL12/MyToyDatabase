#pragma once

#include <sstream>
#include <string>
#include "key_value_store_internal/key_value_store.h"

class InputInterpreter
{
public:
    explicit InputInterpreter(KeyValueStore &keyValueStore) : keyValueStore_(keyValueStore) {}
    void interpret(std::string command) const;

private:
    void interpretGetCommand(std::istringstream &stringsStream) const;
    void interpretPutCommand(std::istringstream &command) const;
    void interpretDeleteCommand(std::istringstream &command) const;
    KeyValueStore &keyValueStore_;
};