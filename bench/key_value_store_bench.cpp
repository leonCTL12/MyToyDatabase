#include <benchmark/benchmark.h>
#include "key_value_store.h"

// Hello world benchmark for KeyValueStore::put
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

// This macro registers the BM_Put function as a benchmark with Google Benchmark's runtime system
// Ensuring that when the benchmark executables runs, BM_Put will be discovered and executed
BENCHMARK(BM_Put);