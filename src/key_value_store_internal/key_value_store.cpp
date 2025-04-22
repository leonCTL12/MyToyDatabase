#include "key_value_store.h"
#include <iostream>

KeyValueStore::KeyValueStore(size_t initial_size)
    // Initialising buckets with n nullptrs
    : buckets(initial_size, nullptr), numBuckets(initial_size), numElements(0)
{
}

KeyValueStore::~KeyValueStore()
{
    std::lock_guard<std::mutex> lock(mutex);
    for (auto &bucket : buckets)
    {
        Node *current = bucket;
        while (current)
        {
            Node *next = current->next;
            delete current;
            current = next;
        }
    }
}

void KeyValueStore::put(const std::string_view key, const std::string_view value)
{
    // No need to check if the key already exists
    //  as we are overwriting the value
    std::lock_guard<std::mutex> lock(mutex);

    if (static_cast<float>(numElements + 1) / numBuckets > maxLoadFactor)
    {
        resize();
    }

    size_t index = hash(key);
    Node *current = buckets[index];
    while (current)
    {
        if (current->key == key)
        {
            current->value = value; // Update the value if the key already exists
            return;
        }
        current = current->next;
    }

    // Insert at the beginning of the linked list
    Node *newNode = new Node(std::string(key), std::string(value));
    newNode->next = buckets[index];
    buckets[index] = newNode;
    numElements++;
}

std::optional<std::string> KeyValueStore::get(const std::string_view key) const
{
    std::lock_guard<std::mutex> lock(mutex);

    size_t index = hash(key);
    Node *current = buckets[index];

    while (current)
    {
        if (current->key == key)
        {
            return current->value; // Return the value if the key is found
        }
        current = current->next;
    }
    return std::nullopt; // Return nullopt if the key is not found
}

bool KeyValueStore::deleteKey(const std::string_view key)
{
    std::lock_guard<std::mutex> lock(mutex);

    size_t index = hash(key);
    Node *current = buckets[index];
    Node *prev = nullptr;

    while (current)
    {
        if (current->key == key)
        {
            if (prev)
            {
                prev->next = current->next; // Bypass the current node
            }
            else
            {
                buckets[index] = current->next; // Remove the first node in the bucket
            }
            delete current;
            numElements--;
            return true; // Return true if the key was found and deleted
        }
        prev = current;
        current = current->next;
    }

    return false; // Return false if the key was not found
}

size_t KeyValueStore::hash(const std::string_view key) const
{
    return std::hash<std::string>()(std::string(key)) % numBuckets;
}

void KeyValueStore::resize()
{
    size_t newNumBuckets = numBuckets * 2;
    if (newNumBuckets == 0)
    {
        newNumBuckets = 16;
    }
    std::vector<Node *> newBuckets(newNumBuckets, nullptr);

    // Rehash all existing keys
    for (Node *bucket : buckets)
    {
        Node *current = bucket;
        while (current)
        {
            Node *next = current->next;
            size_t newIndex = std::hash<std::string>()(current->key) % newNumBuckets;

            // Insert at the beginning of the new linked list
            current->next = newBuckets[newIndex];
            newBuckets[newIndex] = current;

            current = next;
        }
    }

    buckets = std::move(newBuckets);
    numBuckets = newNumBuckets;
}