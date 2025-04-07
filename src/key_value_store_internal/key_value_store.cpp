#include "key_value_store.h"
#include <iostream>

void KeyValueStore::put(const std::string_view key, const std::string_view value)
{
    // No need to check if the key already exists
    //  as we are overwriting the value
    unorderedMap[std::string(key)] = std::string(value);
}

std::optional<std::string> KeyValueStore::get(const std::string_view key) const
{
    auto it = unorderedMap.find(std::string(key));
    if (it != unorderedMap.end())
    {
        return it->second;
    }
    return std::nullopt;
}

bool KeyValueStore::deleteKey(const std::string_view key)
{
    auto it = unorderedMap.find(std::string(key));
    if (it == unorderedMap.end())
    {
        return false;
    }

    unorderedMap.erase(it);
    return true;
}
