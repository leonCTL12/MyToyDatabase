#include <benchmark/benchmark.h>
#include <string>
#include "key_value_store.h"

// Benchmark for KeyValueStore::put
static void BM_Put(benchmark::State &state)
{
    KeyValueStore kv;
    std::string key = "key";
    std::string value = "value";
    for (auto _ : state)
    {
        kv.put(key, value);
    }
}

// Benchmark for KeyValueStore::get
static void BM_Get(benchmark::State &state)
{
    KeyValueStore kv;
    std::string key = "key";
    std::string value = "value";
    kv.put(key, value); // Setup: insert key-value pair
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(kv.get(key));
    }
}

// Benchmark for KeyValueStore::deleteKey
static void BM_DeleteKey(benchmark::State &state)
{
    KeyValueStore kv;
    std::string key = "key";
    std::string value = "value";
    for (auto _ : state)
    {
        state.PauseTiming();
        // The reason why we have to put within the loop is to prevent keep missing after the first iteration
        kv.put(key, value); // Setup: insert key-value pair
        state.ResumeTiming();
        kv.deleteKey(key);
    }
}

// Benchmark for Put-Get sequence
static void BM_PutGet(benchmark::State &state)
{
    KeyValueStore kv;
    std::string key = "key";
    std::string value = "value";
    for (auto _ : state)
    {
        kv.put(key, value);
        benchmark::DoNotOptimize(kv.get(key));
    }
}

// Benchmark for Put-Delete sequence
static void BM_PutDelete(benchmark::State &state)
{
    KeyValueStore kv;
    std::string key = "key";
    std::string value = "value";
    for (auto _ : state)
    {
        kv.put(key, value);
        kv.deleteKey(key);
    }
}

BENCHMARK(BM_Put);
BENCHMARK(BM_Get);
BENCHMARK(BM_DeleteKey);
BENCHMARK(BM_PutGet);
BENCHMARK(BM_PutDelete);