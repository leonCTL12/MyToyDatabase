#include "key_value_store.h"
#include <iostream>

KeyValueStore::KeyValueStore(size_t initial_size)
    // Initialising buckets with n nullptrs
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
            current->release(); // Decrement ref count
            current = next;
        }
    }
}

void KeyValueStore::put(const std::string_view key, const std::string_view value)
{
    size_t index = hash(key);

    // CAS Retry Loop
    while (true)
    {
        Node *head = buckets[index].load(std::memory_order_acquire);
        if (!head)
        {
            Node *newNode = new Node(std::string(key), std::string(value));
            if (buckets[index].compare_exchange_strong(head, newNode, std::memory_order_release))
            {
                numElements.fetch_add(1, std::memory_order_relaxed); // Increment the number of elements
                return;
            }
            delete newNode; // If CAS fails, we need to delete the new node
            continue;       // Retry
        }

        head->retain();
        Node *current = head;
        Node *prev = nullptr;

        // Traverse list to find key
        while (current && current->key != key)
        {
            prev = current;
            current = current->next.load(std::memory_order_acquire);
            if (current)
            {
                current->retain(); // Increment ref count for next node
            }
            prev->release(); // Decrement ref count for previous node
        }

        Node *newNode = new Node(std::string(key), std::string(value));

        // Key not found, insert at head
        if (!current)
        {
            newNode->next.store(head, std::memory_order_relaxed);
            if (buckets[index].compare_exchange_strong(head, newNode, std::memory_order_release, std::memory_order_acquire))
            {
                numElements.fetch_add(1, std::memory_order_relaxed); // Increment the number of elements
                if (prev)
                {
                    prev->release();
                }
                return;
            }
            // If CAS fails, we need to delete the new node
            delete newNode;
            if (prev)
            {
                prev->release(); // Decrement ref count for previous node
            }
            continue; // Retry
        }
        else
        {
            // Key found, update value
            newNode->next.store(current->next.load(std::memory_order_acquire), std::memory_order_relaxed);
            if (prev)
            {
                if (prev->next.compare_exchange_strong(current, newNode, std::memory_order_release, std::memory_order_acquire))
                {
                    current->release(); // Decrement ref count for current node
                    prev->release();    // Decrement ref count for previous node
                    return;
                }
                delete newNode;     // If CAS fails, we need to delete the new node
                current->release(); // Decrement ref count for current node
                prev->release();    // Decrement ref count for previous node
            }
            else
            {
                if (buckets[index].compare_exchange_strong(current, newNode, std::memory_order_release))
                {
                    head->release(); // Decrement ref count for current node
                    return;
                }
                delete newNode;  // If CAS fails, we need to delete the new node
                head->release(); // Decrement ref count for current node
            }
            continue;
        }
    }
}

std::optional<std::string> KeyValueStore::get(const std::string_view key) const
{
    size_t index = hash(key);
    Node *current = buckets[index].load(std::memory_order_acquire);

    if (current)
    {
        current->retain(); // Increment ref count
    }

    while (current)
    {
        if (current->key == key)
        {
            std::string value = current->value; // Copy the value
            current->release();
            return value; // Return the value if the key is found
        }
        Node *next = current->next.load(std::memory_order_acquire);
        // TODO: figure out what if next is released here, before we even call retain();
        if (next)
        {
            next->retain(); // Increment ref count for next node
        }
        current->release(); // Decrement ref count for current node
        current = next;     // Move to the next node
    }
    return std::nullopt; // Return nullopt if the key is not found
}

bool KeyValueStore::deleteKey(const std::string_view key)
{
    size_t index = hash(key);

    // CAS Retry Loop
    while (true)
    {
        Node *head = buckets[index].load(std::memory_order_acquire);
        if (!head)
        {
            return false; // Key not found
        }
        head->retain();
        Node *current = head;
        Node *prev = nullptr;

        while (current && current->key != key)
        {
            prev = current;
            current = current->next.load(std::memory_order_acquire);
            if (current)
            {
                current->retain(); // Increment ref count for next node
            }
            prev->release(); // Decrement ref count for previous node
        }

        if (!current) // Key not found
        {
            if (prev)
            {
                prev->release(); // Decrement ref count for previous node
            }
            return false;
        }

        // Perform actual delete logic
        Node *next = current->next.load(std::memory_order_acquire);
        // The node deleted is not the head of the list
        if (prev)
        {
            if (prev->next.compare_exchange_strong(current, next, std::memory_order_release, std::memory_order_acquire))
            {
                current->release();
                prev->release();
                numElements.fetch_sub(1, std::memory_order_relaxed); // Approximate count
                return true;
            }
            current->release();
            prev->release();
        }
        else // The node deleted is the head of the list
        {
            if (buckets[index].compare_exchange_strong(current, next,
                                                       std::memory_order_release,
                                                       std::memory_order_acquire))
            {
                head->release();                                     // Decrement ref count for current node
                numElements.fetch_sub(1, std::memory_order_relaxed); // Approximate count
                return true;
            }
            head->release(); // Decrement ref count for current node
        }
        continue;
    }
}

size_t KeyValueStore::hash(const std::string_view key) const
{
    return std::hash<std::string_view>()(key) % numBuckets;
}

void KeyValueStore::resize()
{
    // TODO: bring back resize later with consistent hashing
}