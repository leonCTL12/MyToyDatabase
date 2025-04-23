#pragma once
#include <string_view>
#include <optional>
#include <string>
#include <mutex>
#include <vector>
#include <atomic>

class KeyValueStore
{
public:
    explicit KeyValueStore(size_t initial_size = 1000000);
    ~KeyValueStore();
    void put(const std::string_view key, const std::string_view value);
    // Why return string instead of string_view?
    // string_view is non-owning and can be invalidated if the underlying string is modified or destroyed.
    std::optional<std::string> get(const std::string_view key) const;
    bool deleteKey(const std::string_view key); // delete is a keyword in C++, so we use deleteKey

private:
    struct Node
    {
        std::string key;
        std::string value;
        std::atomic<Node *> next;
        Node(const std::string &k, const std::string &v) : key(k), value(v), next(nullptr) {}
    };

    std::vector<std::atomic<Node *>> buckets;
    size_t numBuckets;
    std::atomic<size_t> numElements;
    // float maxLoadFactor = 0.75f; // Maximum load factor before resizing

    size_t hash(const std::string_view key) const;
    void resize();
};