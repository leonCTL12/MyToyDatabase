#pragma once
#include <string_view>
#include <unordered_map>
#include <optional>
#include <string>

class KeyValueStore
{
public:
    explicit KeyValueStore() {}
    void put(const std::string_view key, const std::string_view value);
    // Why return string instead of string_view?
    // string_view is non-owning and can be invalidated if the underlying string is modified or destroyed.
    std::optional<std::string> get(const std::string_view key) const;
    bool deleteKey(const std::string_view key); // delete is a keyword in C++, so we use deleteKey

private:
    std::unordered_map<std::string, std::string> unorderedMap;
};