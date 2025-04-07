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
    std::optional<std::string_view> get(const std::string_view key) const;

private:
    std::unordered_map<std::string, std::string>
        unorderedMap;
};