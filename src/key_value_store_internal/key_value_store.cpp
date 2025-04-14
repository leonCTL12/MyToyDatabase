#include "key_value_store.h"
#include <iostream>

void KeyValueStore::put(const std::string_view key, const std::string_view value)
{
    // No need to check if the key already exists
    //  as we are overwriting the value
    std::lock_guard<std::mutex> lock(mutex);
    unorderedMap[std::string(key)] = std::string(value);
}

std::optional<std::string> KeyValueStore::get(const std::string_view key) const
{
    std::lock_guard<std::mutex> lock(mutex);

    auto it = unorderedMap.find(std::string(key));
    if (it != unorderedMap.end())
    {
        return it->second;
    }
    return std::nullopt;
}

bool KeyValueStore::deleteKey(const std::string_view key)
{
    std::lock_guard<std::mutex> lock(mutex);

    auto it = unorderedMap.find(std::string(key));
    if (it == unorderedMap.end())
    {
        return false;
    }

    unorderedMap.erase(it);
    return true;
}
