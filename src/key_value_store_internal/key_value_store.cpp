#include "key_value_store.h"
#include <thread>
#include <algorithm>
#include <stdexcept>
#include <iostream> // For debugging (optional)

// Thread-local storage
thread_local HazardPointer *localHazardPointers = nullptr;
thread_local std::vector<Node *> retiredNodes;
thread_local bool isRegistered = false;

std::atomic<HazardPointer *> globalHazardPointers[MAX_THREADS] = {};
std::mutex globalHazardPointersMutex;

HazardPointer *KeyValueStore::HazardPointerManager::getHazardPointers()
{
    if (!localHazardPointers)
    {
        localHazardPointers = new HazardPointer();
        if (!isRegistered)
        {
            registerThread();
            isRegistered = true;
        }
    }
    return localHazardPointers;
}

void KeyValueStore::HazardPointerManager::registerThread()
{
    static std::atomic<int> registrationCount{0};
    std::lock_guard<std::mutex> lock(globalHazardPointersMutex);
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        if (globalHazardPointers[i].load(std::memory_order_relaxed) == nullptr)
        {
            globalHazardPointers[i].store(localHazardPointers, std::memory_order_release);
            int count = registrationCount.fetch_add(1, std::memory_order_relaxed) + 1;
            return;
        }
    }
    int count = registrationCount.load(std::memory_order_relaxed);
    throw std::runtime_error("Too many threads, attempted: " + std::to_string(count + 1) +
                             ", max: " + std::to_string(MAX_THREADS));
}

void KeyValueStore::HazardPointerManager::unregisterThread()
{
    std::lock_guard<std::mutex> lock(globalHazardPointersMutex);
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        if (globalHazardPointers[i].load(std::memory_order_relaxed) == localHazardPointers)
        {
            globalHazardPointers[i].store(nullptr, std::memory_order_release);
            delete localHazardPointers;
            localHazardPointers = nullptr;
            return;
        }
    }
}

std::atomic<HazardPointer *> *KeyValueStore::HazardPointerManager::getGlobalHazardPointers()
{
    return globalHazardPointers;
}

KeyValueStore::KeyValueStore(size_t initial_size)
    : buckets(initial_size), numBuckets(initial_size), numElements(0)
{
    for (auto &bucket : buckets)
    {
        bucket.store(nullptr, std::memory_order_relaxed);
    }
}

KeyValueStore::~KeyValueStore()
{
    for (auto &bucket : buckets)
    {
        Node *current = bucket.load(std::memory_order_acquire);
        while (current)
        {
            Node *next = current->next.load(std::memory_order_acquire);
            delete current;
            current = next;
        }
    }
}

void KeyValueStore::put(const std::string_view key, const std::string_view value)
{
    size_t index = hash(key);
    HazardPointer *hp = HazardPointerManager::getHazardPointers();

    while (true)
    {
        Node *head = buckets[index].load(std::memory_order_acquire);
        hp->setHazardPointer(0, head);

        if (!head)
        {
            Node *newNode = new Node(std::string(key), std::string(value));
            if (buckets[index].compare_exchange_strong(head, newNode, std::memory_order_release))
            {
                numElements.fetch_add(1, std::memory_order_relaxed);
                hp->clearHazardPointer(0);
                if (retiredNodes.size() >= RECLAIM_THRESHOLD)
                {
                    reclaimRetiredNodes();
                }
                return;
            }
            delete newNode;
            hp->clearHazardPointer(0);
            continue;
        }

        Node *current = head;
        Node *prev = nullptr;
        hp->setHazardPointer(1, current);

        while (current && current->key != key)
        {
            prev = current;
            Node *next = current->next.load(std::memory_order_acquire);
            hp->setHazardPointer(0, next);
            hp->clearHazardPointer(1);
            current = next;
            hp->setHazardPointer(1, current);
        }

        if (current)
        {
            Node *newNode = new Node(std::string(key), std::string(value));
            newNode->next.store(current->next.load(std::memory_order_acquire), std::memory_order_relaxed);
            bool success = false;
            if (prev)
            {
                if (prev->next.compare_exchange_strong(current, newNode, std::memory_order_release))
                {
                    success = true;
                }
            }
            else
            {
                if (buckets[index].compare_exchange_strong(current, newNode, std::memory_order_release))
                {
                    success = true;
                }
            }
            if (success)
            {
                retireNode(current);
                hp->clearHazardPointer(0);
                hp->clearHazardPointer(1);
                if (retiredNodes.size() >= RECLAIM_THRESHOLD)
                {
                    reclaimRetiredNodes();
                }
                return;
            }
            delete newNode;
            hp->clearHazardPointer(0);
            hp->clearHazardPointer(1);
            continue;
        }

        Node *newNode = new Node(std::string(key), std::string(value));
        newNode->next.store(head, std::memory_order_relaxed);
        if (buckets[index].compare_exchange_strong(head, newNode, std::memory_order_release))
        {
            numElements.fetch_add(1, std::memory_order_relaxed);
            hp->clearHazardPointer(0);
            hp->clearHazardPointer(1);
            if (retiredNodes.size() >= RECLAIM_THRESHOLD)
            {
                reclaimRetiredNodes();
            }
            return;
        }
        delete newNode;
        hp->clearHazardPointer(0);
        hp->clearHazardPointer(1);
        continue;
    }
}

std::optional<std::string> KeyValueStore::get(const std::string_view key) const
{
    size_t index = hash(key);
    HazardPointer *hp = HazardPointerManager::getHazardPointers();

    while (true)
    {
        // Step 1: Load and protect head
        Node *head = buckets[index].load(std::memory_order_acquire);
        if (!head)
        {
            return std::nullopt;
        }
        hp->setHazardPointer(0, head);
        // Verify head is still valid
        if (buckets[index].load(std::memory_order_acquire) != head)
        {
            hp->clearHazardPointer(0);
            continue; // Head changed, retry
        }

        Node *current = head;
        Node *prev = nullptr;

        while (current)
        {
            // Step 2: Protect current
            hp->setHazardPointer(1, current);

            // Step 3: Load next while current is protected
            Node *next = current->next.load(std::memory_order_acquire);

            // Step 4: Immediately protect next before any access
            hp->setHazardPointer(0, next);

            // Step 5: Validate that current is still linked
            if (prev && prev->next.load(std::memory_order_acquire) != current)
            {
                hp->clearHazardPointer(0);
                hp->clearHazardPointer(1);
                break; // Current was unlinked, retry
            }

            // Step 6: Check key match
            if (current->key == key)
            {
                std::string value = current->value;
                hp->clearHazardPointer(0);
                hp->clearHazardPointer(1);
                return value;
            }

            // Step 7: Advance to next
            prev = current;
            current = next;

            // Clear hazard pointer 1 only after next is protected
            hp->clearHazardPointer(1);
        }

        // Clear remaining hazard pointer
        hp->clearHazardPointer(0);

        // If we broke due to unlinking, retry
        if (current)
        {
            continue;
        }
        return std::nullopt;
    }
}

bool KeyValueStore::deleteKey(const std::string_view key)
{
    size_t index = hash(key);
    HazardPointer *hp = HazardPointerManager::getHazardPointers();

    while (true)
    {
        Node *head = buckets[index].load(std::memory_order_acquire);
        hp->setHazardPointer(0, head);

        if (!head)
        {
            hp->clearHazardPointer(0);
            return false;
        }

        Node *current = head;
        Node *prev = nullptr;

        while (current && current->key != key)
        {
            prev = current;
            Node *next = current->next.load(std::memory_order_acquire);
            hp->setHazardPointer(1, next);
            current = next;
        }

        if (!current)
        {
            hp->clearHazardPointer(0);
            hp->clearHazardPointer(1);
            return false;
        }

        Node *next = current->next.load(std::memory_order_acquire);
        bool success = false;
        if (prev)
        {
            if (prev->next.compare_exchange_strong(current, next, std::memory_order_release))
            {
                success = true;
            }
        }
        else
        {
            if (buckets[index].compare_exchange_strong(current, next, std::memory_order_release))
            {
                success = true;
            }
        }
        if (success)
        {
            retireNode(current);
            numElements.fetch_sub(1, std::memory_order_relaxed);
            hp->clearHazardPointer(0);
            hp->clearHazardPointer(1);
            if (retiredNodes.size() >= RECLAIM_THRESHOLD)
            {
                reclaimRetiredNodes();
            }
            return true;
        }
        hp->clearHazardPointer(0);
        hp->clearHazardPointer(1);
        continue;
    }
}

void KeyValueStore::retireNode(Node *node) const
{
    retiredNodes.push_back(node);
}

void KeyValueStore::reclaimRetiredNodes() const
{
    std::vector<Node *> toDelete;
    for (auto node : retiredNodes)
    {
        bool safe = true;
        auto *globalHPs = HazardPointerManager::getGlobalHazardPointers();
        for (int i = 0; i < MAX_THREADS; ++i)
        {
            auto *hp = globalHPs[i].load(std::memory_order_acquire);
            if (hp)
            {
                for (int j = 0; j < MAX_HAZARD_POINTERS; ++j)
                {
                    if (hp->getHazardPointer(j) == node)
                    {
                        safe = false;
                        break;
                    }
                }
            }
            if (!safe)
                break;
        }
        if (safe)
        {
            toDelete.push_back(node);
        }
    }
    for (auto node : toDelete)
    {
        delete node;
        retiredNodes.erase(std::remove(retiredNodes.begin(), retiredNodes.end(), node), retiredNodes.end());
    }
}

size_t KeyValueStore::hash(const std::string_view key) const
{
    return std::hash<std::string_view>{}(key) % numBuckets;
}