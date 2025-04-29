#pragma once
#include <string_view>
#include <optional>
#include <string>
#include <vector>
#include <atomic>
#include <mutex>

constexpr int MAX_HAZARD_POINTERS = 2;
constexpr int MAX_THREADS = 64;
constexpr size_t RECLAIM_THRESHOLD = 2000; // Increased further to reduce reclamation frequency

struct Node
{
    std::string key;
    std::string value;
    std::atomic<Node *> next;
    Node(const std::string &k, const std::string &v) : key(k), value(v), next(nullptr) {}
};

class HazardPointer
{
public:
    HazardPointer()
    {
        for (auto &hp : hazardPointers)
        {
            hp.store(nullptr, std::memory_order_relaxed);
        }
    }
    void setHazardPointer(int index, Node *node)
    {
        hazardPointers[index].store(node, std::memory_order_release);
    }
    Node *getHazardPointer(int index) const
    {
        return hazardPointers[index].load(std::memory_order_acquire);
    }
    void clearHazardPointer(int index)
    {
        hazardPointers[index].store(nullptr, std::memory_order_release);
    }

private:
    std::atomic<Node *> hazardPointers[MAX_HAZARD_POINTERS];
};

class KeyValueStore
{
public:
    explicit KeyValueStore(size_t initial_size = 1000000);
    ~KeyValueStore();
    KeyValueStore(const KeyValueStore &) = delete;
    KeyValueStore &operator=(const KeyValueStore &) = delete;
    void put(const std::string_view key, const std::string_view value);
    std::optional<std::string> get(const std::string_view key) const;
    bool deleteKey(const std::string_view key);
    class HazardPointerManager
    {
    public:
        static HazardPointer *getHazardPointers();
        static void registerThread();
        static void unregisterThread();
        static std::atomic<HazardPointer *> *getGlobalHazardPointers();
};

private:
    std::vector<std::atomic<Node *>> buckets;
    size_t numBuckets;
    std::atomic<size_t> numElements;

    size_t hash(const std::string_view key) const;
    void retireNode(Node *node) const;
    void reclaimRetiredNodes() const;
};