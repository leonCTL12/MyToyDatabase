#include <gtest/gtest.h>
#include "key_value_store_internal/key_value_store.h"
#include <thread>

// the reason why this test failed is that...
// when 1 thread triggered a resizing
// and another thread tried to put a new key, that change may be lost or corrupted
// coz the map's internal state (buckets, pointers, etc) is being modified concurrently without synchronisation
TEST(KeyValueStoreTest, StressConcurrentPuts)
{
    KeyValueStore kvStore;
    const int numThreads = 20;
    const int operationsPerThread = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, operationsPerThread]()
                             {
            for (int j = 0; j < operationsPerThread; ++j) {
                std::string key = "key" + std::to_string(i) + "_" + std::to_string(j);
                std::string value = "value" + std::to_string(j);
                kvStore.put(key, value);
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    // Verify all keys
    for (int i = 0; i < numThreads; ++i)
    {
        for (int j = 0; j < operationsPerThread; ++j)
        {
            std::string key = "key" + std::to_string(i) + "_" + std::to_string(j);
            auto value = kvStore.get(key);
            ASSERT_TRUE(value.has_value());
            EXPECT_EQ(*value, "value" + std::to_string(j));
        }
    }
}

TEST(KeyValueStoreTest, ConcurrentGets)
{
    KeyValueStore kvStore;
    const int numThreads = 10;

    // Pre-populate the store
    for (int i = 0; i < numThreads; ++i)
    {
        std::string key = "key" + std::to_string(i);
        std::string value = "value" + std::to_string(i);
        kvStore.put(key, value);
    }

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i]()
                             {
            std::string key = "key" + std::to_string(i);
            auto value = kvStore.get(key);
            ASSERT_TRUE(value.has_value());
            EXPECT_EQ(*value, "value" + std::to_string(i)); });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

TEST(KeyValueStoreTest, StressConcurrentUpdates)
{
    KeyValueStore kvStore;
    std::string key = "sharedKey";
    kvStore.put(key, "initial");

    const int numThreads = 50;
    const int updatesPerThread = 1000; // Increased for more contention
    std::vector<std::thread> threads;
    std::vector<std::string> expectedValues;

    // Generate expected values
    for (int i = 0; i < numThreads; ++i)
    {
        for (int j = 0; j < updatesPerThread; ++j)
        {
            expectedValues.push_back("value" + std::to_string(i) + "_" + std::to_string(j));
        }
    }

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, key, i, updatesPerThread]()
                             {
            for (int j = 0; j < updatesPerThread; ++j) {
                std::string value = "value" + std::to_string(i) + "_" + std::to_string(j);
                kvStore.put(key, value);
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    auto value = kvStore.get(key);
    ASSERT_TRUE(value.has_value());
    // Stronger assertion: final value must be one of the expected updates
    bool validValue = std::find(expectedValues.begin(), expectedValues.end(), *value) != expectedValues.end();
    EXPECT_TRUE(validValue) << "Final value '" << *value << "' is not one of the expected updates";
}
TEST(KeyValueStoreTest, ConcurrentDeleteAndGet)
{
    KeyValueStore kvStore;
    std::string key = "keyToDelete";
    kvStore.put(key, "value");

    std::atomic<bool> deleteDone(false);

    std::thread t1([&kvStore, key, &deleteDone]()
                   {
        kvStore.deleteKey(key);
        deleteDone = true; });

    std::thread t2([&kvStore, key, &deleteDone]()
                   {
        while (!deleteDone) {
            auto val = kvStore.get(key);
            if (val.has_value()) {
                EXPECT_EQ(*val, "value");
            } else {
                EXPECT_TRUE(deleteDone);
            }
        } });

    t1.join();
    t2.join();
}

TEST(KeyValueStoreTest, StressConcurrentPutAndGet)
{
    KeyValueStore kvStore;
    std::string key = "keyStress";
    kvStore.put(key, "initial");

    const int numThreads = 50; // Increased threads
    const int operationsPerThread = 1000;
    std::vector<std::thread> threads;
    std::vector<std::string> expectedValues = {"initial"};

    // Generate expected values for puts
    for (int i = 0; i < numThreads; ++i)
    {
        if (i % 2 == 0)
        {
            for (int j = 0; j < operationsPerThread; ++j)
            {
                expectedValues.push_back("value" + std::to_string(i) + "_" + std::to_string(j));
            }
        }
    }

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, key, i, operationsPerThread]()
                             {
            for (int j = 0; j < operationsPerThread; ++j) {
                if (i % 2 == 0) {
                    std::string value = "value" + std::to_string(i) + "_" + std::to_string(j);
                    kvStore.put(key, value);
                } else {
                    auto value = kvStore.get(key);
                    // Check during execution to catch issues
                    if (value.has_value()) {
                        // In a real test, we'd need thread-safe checking, but for demo:
                        // Expect value to be initial or one of the puts
                        bool valid = *value == "initial" ||
                                     value->find("value") == 0; // Rough check for format
                        EXPECT_TRUE(valid) << "Invalid value during get: " << *value;
                    }
                }
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    auto value = kvStore.get(key);
    ASSERT_TRUE(value.has_value());
    // Stronger assertion: final value must be initial or one of the puts
    bool validValue = std::find(expectedValues.begin(), expectedValues.end(), *value) != expectedValues.end();
    EXPECT_TRUE(validValue) << "Final value '" << *value << "' is not one of the expected values";
}
// The store remains consistent, with no crashes or undefined behavior.
TEST(KeyValueStoreTest, ConcurrentMixedKeysStress)
{
    KeyValueStore kvStore;
    const int numThreads = 30;
    const int operationsPerThread = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, operationsPerThread]()
                             {
            for (int j = 0; j < operationsPerThread; ++j) {
                // Use a small set of keys to increase contention
                std::string key = "key" + std::to_string(j % 10);
                if (j % 3 == 0) {
                    kvStore.put(key, "value" + std::to_string(j));
                } else if (j % 3 == 1) {
                    kvStore.get(key);
                } else {
                    kvStore.deleteKey(key);
                }
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    // Verify some keys (we can't predict exact state, but check for consistency)
    for (int j = 0; j < 10; ++j)
    {
        std::string key = "key" + std::to_string(j);
        auto value = kvStore.get(key);
        // If key exists, it should have a valid value
        if (value.has_value())
        {
            EXPECT_TRUE(value->find("value") == 0); // Starts with "value"
        }
    }
}

TEST(KeyValueStoreTest, HighContentionSingleKey)
{
    KeyValueStore kvStore;
    std::string key = "contendedKey";
    kvStore.put(key, "initial");

    const int numThreads = 20;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, key, i]()
                             {
            if (i % 2 == 0) {
                kvStore.put(key, "value" + std::to_string(i));
            } else {
                kvStore.get(key);
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    auto value = kvStore.get(key);
    ASSERT_TRUE(value.has_value());
    // The final value should be one of the put values
    EXPECT_TRUE(*value >= "value0" && *value <= "value19");
}

TEST(KeyValueStoreTest, ConcurrentEdgeCases)
{
    KeyValueStore kvStore;
    const int numThreads = 5;

    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i]()
                             {
            std::string key = (i % 2 == 0) ? "" : "key with space!";
            std::string value = (i % 2 == 0) ? "" : "value@#$%";
            kvStore.put(key, value);
            auto val = kvStore.get(key);
            if (val.has_value()) {
                EXPECT_EQ(*val, value);
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
}

TEST(KeyValueStoreTest, ConcurrentDeletesAndPuts)
{
    KeyValueStore kvStore;
    const int numThreads = 10;
    const int opsPerThread = 1000;
    std::vector<std::thread> threads;

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, opsPerThread]()
                             {
            for (int j = 0; j < opsPerThread; ++j) {
                std::string key = "key" + std::to_string(i) + "_" + std::to_string(j);
                if (j % 2 == 0) {
                    kvStore.put(key, "value");
                } else {
                    kvStore.deleteKey(key);
                }
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }
    // Check for crashes or basic consistency
}

TEST(KeyValueStoreTest, ConcurrentResizingWithReads)
{
    KeyValueStore kvStore;
    const int numThreads = 20;
    const int opsPerThread = 1000;
    std::vector<std::thread> threads;

    // Pre-populate
    for (int i = 0; i < 100; ++i)
    {
        kvStore.put("pre" + std::to_string(i), "value");
    }

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, opsPerThread]()
                             {
            for (int j = 0; j < opsPerThread; ++j) {
                if (i % 2 == 0) {
                    std::string key = "key" + std::to_string(i) + "_" + std::to_string(j);
                    kvStore.put(key, "value");
                } else {
                    std::string key = "pre" + std::to_string(j % 100);
                    kvStore.get(key);
                }
            } });
    }

    for (auto &thread : threads)
    {
        thread.join();
    }

    // Verify pre-populated keys
    for (int i = 0; i < 100; ++i)
    {
        auto value = kvStore.get("pre" + std::to_string(i));
        ASSERT_TRUE(value.has_value());
        EXPECT_EQ(*value, "value");
    }
}

// Test to catch use-after-free in long-running concurrent put (updates) and get
TEST(KeyValueStoreTest, ConcurrentPutAndGetUseAfterFree)
{
    KeyValueStore kvStore;
    const int numThreads = 16; // High contention
    std::vector<std::thread> threads;
    std::atomic<bool> stop(false);

    // Pre-populate with a single key to ensure updates
    kvStore.put("key", "initial_value");

    // Launch threads: half perform put (updates), half perform get
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, &stop]()
                             {
            while (!stop) {
                if (i % 2 == 0) {
                    // Put: Update the key to trigger delete current
                    kvStore.put("key", "value_" + std::to_string(i));
                } else {
                    // Get: Traverse, potentially accessing freed nodes
                    kvStore.get("key");
                    std::this_thread::yield(); // Slow traversal to increase overlap
                }
            } });
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    stop = true;

    // Join all threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Verify key existence (optional, to ensure execution)
    auto value = kvStore.get("key");
    ASSERT_TRUE(value.has_value()) << "Key 'key' should exist";
    EXPECT_TRUE(value->find("value_") == 0) << "Unexpected value: " << *value;
}

// Test to catch use-after-free in concurrent put (updates) and deleteKey
TEST(KeyValueStoreTest, ConcurrentPutAndDeleteUseAfterFree)
{
    KeyValueStore kvStore;
    const int numThreads = 20;
    const int opsPerThread = 1000;
    std::vector<std::thread> threads;

    // Pre-populate with a small set of keys to ensure updates
    for (int i = 0; i < 10; ++i)
    {
        kvStore.put("key" + std::to_string(i), "initial_value");
    }

    // Launch threads: half perform put (updates), half perform deleteKey
    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, opsPerThread]()
                             {
            for (int j = 0; j < opsPerThread; ++j) {
                // Use a small key set to target the same buckets
                std::string key = "key" + std::to_string(j % 10);
                if (i % 2 == 0) {
                    // Put: Update or insert to trigger delete current
                    kvStore.put(key, "value_" + std::to_string(j));
                } else {
                    // Delete: Remove keys, triggering delete current
                    kvStore.deleteKey(key);
                    // Re-insert to keep the key set active
                    kvStore.put(key, "reinsert_" + std::to_string(j));
                }
            } });
    }

    // Join all threads
    for (auto &thread : threads)
    {
        thread.join();
    }

    // Verify some keys (optional, as deletion may remove some)
    for (int i = 0; i < 10; ++i)
    {
        auto value = kvStore.get("key" + std::to_string(i));
        if (value.has_value())
        {
            // Values may be from put or reinsert
            EXPECT_TRUE(value->find("value_") == 0 || value->find("reinsert_") == 0)
                << "Unexpected value for key" << i;
        }
    }
}

TEST(KeyValueStoreTest, LongRunningStress)
{
    KeyValueStore kvStore;
    const int numThreads = 10;
    std::vector<std::thread> threads;
    std::atomic<bool> stop(false);

    for (int i = 0; i < numThreads; ++i)
    {
        threads.emplace_back([&kvStore, i, &stop]()
                             {
            while (!stop) {
                std::string key = "key" + std::to_string(i);
                kvStore.put(key, "value");
                kvStore.get(key);
                kvStore.deleteKey(key);
            } });
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    stop = true;
    for (auto &thread : threads)
    {
        thread.join();
    }
}

TEST(KeyValueStoreTest, DestructorAfterOperations)
{
    KeyValueStore *kvStore = new KeyValueStore();
    kvStore->put("key", "value");
    std::thread t([&]()
                  {
                      auto val = kvStore->get("key"); // Access during destruction
                  });
    delete kvStore; // Destructor runs
    t.join();       // May crash if not thread-safe
}