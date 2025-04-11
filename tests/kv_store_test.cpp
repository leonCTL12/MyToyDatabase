#include <gtest/gtest.h>
#include "key_value_store_internal/key_value_store.h"

TEST(KeyValueStoreTest, PutAndGet)
{
    KeyValueStore kvStore;
    kvStore.put("key1", "value1");
    auto value = kvStore.get("key1");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "value1");
}
TEST(KeyValueStoreTest, PutAndGetMultiple)
{
    KeyValueStore kvStore;
    kvStore.put("key1", "value1");
    kvStore.put("key2", "value2");
    auto value1 = kvStore.get("key1");
    auto value2 = kvStore.get("key2");
    EXPECT_TRUE(value1.has_value());
    EXPECT_EQ(*value1, "value1");
    EXPECT_TRUE(value2.has_value());
    EXPECT_EQ(*value2, "value2");
}

TEST(KeyValueStoreTest, UpdateValue)
{
    KeyValueStore kvStore;
    kvStore.put("key1", "value1");
    kvStore.put("key1", "new_value1");
    auto value = kvStore.get("key1");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "new_value1");
}
TEST(KeyValueStoreTest, GetNonExistentKey)
{
    KeyValueStore kvStore;
    auto value = kvStore.get("non_existent_key");
    EXPECT_FALSE(value.has_value());
}

TEST(KeyValueStoreTest, EmptyKey)
{
    KeyValueStore kvStore;
    kvStore.put("", "value");
    auto value = kvStore.get("");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "value");
}

TEST(KeyValueStoreTest, EmptyValue)
{
    KeyValueStore kvStore;
    kvStore.put("key", "");
    auto value = kvStore.get("key");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "");
}

TEST(KeyValueStoreTest, SpecialCharacters)
{
    KeyValueStore kvStore;
    kvStore.put("key with space!", "value@#$%é");
    auto value = kvStore.get("key with space!");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "value@#$%é");
}

TEST(KeyValueStoreTest, LongKeyValue)
{
    KeyValueStore kvStore;
    std::string longKey(1000, 'k');   // 1000 'k's
    std::string longValue(1000, 'v'); // 1000 'v's
    kvStore.put(longKey, longValue);
    auto value = kvStore.get(longKey);
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, longValue);
}

TEST(KeyValueStoreTest, CaseSensitivity)
{
    KeyValueStore kvStore;
    kvStore.put("key1", "value1");
    auto valueLower = kvStore.get("key1");
    auto valueUpper = kvStore.get("Key1");
    EXPECT_TRUE(valueLower.has_value());
    EXPECT_EQ(*valueLower, "value1");
    // Assuming case-sensitive; adjust expectation if case-insensitive
    EXPECT_FALSE(valueUpper.has_value());
}

TEST(KeyValueStoreTest, LeadingTrailingSpaces)
{
    KeyValueStore kvStore;
    kvStore.put("key1 ", "value1"); // Trailing space
    kvStore.put("key1", "value2");  // No space
    auto valueWithSpace = kvStore.get("key1 ");
    auto valueNoSpace = kvStore.get("key1");
    EXPECT_TRUE(valueWithSpace.has_value());
    EXPECT_EQ(*valueWithSpace, "value1");
    EXPECT_TRUE(valueNoSpace.has_value());
    EXPECT_EQ(*valueNoSpace, "value2");
}

TEST(KeyValueStoreTest, MultipleUpdates)
{
    KeyValueStore kvStore;
    kvStore.put("key1", "value1");
    kvStore.put("key1", "value2");
    kvStore.put("key1", "value3");
    auto value = kvStore.get("key1");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "value3");
}

TEST(KeyValueStoreTest, SameKeyValueMultiplePuts)
{
    KeyValueStore kvStore;
    kvStore.put("key1", "value1");
    kvStore.put("key1", "value1");
    auto value = kvStore.get("key1");
    EXPECT_TRUE(value.has_value());
    EXPECT_EQ(*value, "value1");
}
