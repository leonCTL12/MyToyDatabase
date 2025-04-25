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
        Node *current = bucket.load(std::memory_order_relaxed);
        while (current)
        {
            Node *next = current->next.load(std::memory_order_relaxed);
            delete current;
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
        Node *current = head;

        // Traverse list to find key
        while (current && current->key != key)
        {
            current = current->next.load(std::memory_order_acquire);
        }

        // Key not found, insert at head
        if (!current)
        {
            Node *newNode = new Node(std::string(key), std::string(value));
            newNode->next.store(head, std::memory_order_relaxed);
            if (buckets[index].compare_exchange_strong(head, newNode, std::memory_order_release, std::memory_order_acquire))
            {
                numElements.fetch_add(1, std::memory_order_relaxed); // Increment the number of elements
                break;
            }
            else
            {
                // If CAS fails, we need to delete the new node
                delete newNode;
            }
        }
        else
        {
            // Key found, update value
            Node *newNode = new Node(std::string(key), std::string(value));
            newNode->next.store(current->next.load(std::memory_order_acquire), std::memory_order_relaxed);
            if (buckets[index].compare_exchange_strong(head, newNode, std::memory_order_release, std::memory_order_acquire))
            {
                // TODO: fix this
                delete current;
                break;
            }
            else
            {
                // If CAS fails, we need to delete the new node
                // because it was not inserted into the list
                delete newNode;
            }
        }
    }
}

std::optional<std::string> KeyValueStore::get(const std::string_view key) const
{
    size_t index = hash(key);
    Node *current = buckets[index].load(std::memory_order_acquire);

    while (current)
    {
        if (current->key == key)
        {
            return current->value; // Return the value if the key is found
        }
        current = current->next.load(std::memory_order_acquire);
    }
    return std::nullopt; // Return nullopt if the key is not found
}

bool KeyValueStore::deleteKey(const std::string_view key)
{
    size_t index = hash(key);

    // CAS Retry Loop
    while (true)
    {
        Node *current = buckets[index].load(std::memory_order_acquire);
        Node *prev = nullptr;

        while (current && current->key != key)
        {
            prev = current;
            current = current->next.load(std::memory_order_acquire);
        }

        if (!current) // Key not found
        {
            return false;
        }

        // Perform actual delete logic
        Node *next = current->next.load(std::memory_order_acquire);
        // The node deleted is not the head of the list
        if (prev)
        {
            if (prev->next.compare_exchange_strong(current, next, std::memory_order_release, std::memory_order_acquire))
            {
                // TODO: fix this
                delete current;
                numElements.fetch_sub(1, std::memory_order_relaxed); // Approximate count
                return true;
            }
        }
        else // The node deleted is the head of the list
        {
            if (buckets[index].compare_exchange_strong(current, next,
                                                       std::memory_order_release,
                                                       std::memory_order_acquire))
            {
                // TODO: fix this
                delete current;
                numElements.fetch_sub(1, std::memory_order_relaxed); // Approximate count
                return true;
            }
        }
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