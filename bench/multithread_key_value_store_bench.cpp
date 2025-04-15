#include <benchmark/benchmark.h>
#include "key_value_store_internal/key_value_store.h"

//[Lifetime]: Assume ThreadRange (1,4) is used and there is only 1 benchmark function
//[Lifetime]: Google Benchmark run 3 configurations: 1 thread, 2 threads, 4 threads
//[Lifetime]: 1 Fixture Construction per configuration per benchmark function(i.e. 3 Fixture Construction in total)
class KeyValueStoreFixture : public benchmark::Fixture
{
public:
    // Shared state
    std::unique_ptr<KeyValueStore> kv;

    KeyValueStoreFixture()
    {
        kv = std::make_unique<KeyValueStore>();
    }

    // 2 reasons to call Setup
    // 1. it can access benchmark::State
    // 2. it can act as the "per-benchmark" function (see below)

    // This is the call sequence, take the case of 4 threads as an example
    // 1. Setup() is called for thread 0,1,2,3 concurrently, no guarantee of order
    // 2. benchmark function is called for thread 0,1,2,3 concurrently

    //[Lifetime]: Set up function is called once per thread
    //[Lifetime]: 1 + 2 + 4 = 7 SetUp calls in total
    void SetUp(const benchmark::State &state) override
    {
        if (state.thread_index() == 0)
        {
            for (int i = 0; i < state.threads(); ++i)
            {
                std::string key = "key" + std::to_string(i);
                kv->put(key, "value");
            }
        }
    }
};

//[Lifetime]: Called once per thread per configuration
// This is the macro to register BM function with the fixture
BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentGet)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(kv->get(key));
    }
}

BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentPut)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    const std::string value = "value";
    for (auto _ : state)
    {
        kv->put(key, value);
    }
}

BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentDeleteKey)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    for (auto _ : state)
    {
        kv->deleteKey(key);
        state.PauseTiming();   // we pause, becuase everything within this loop is benchmarking
        kv->put(key, "value"); // Re-insert for next iteration
        state.ResumeTiming();
    }
}

BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentPutGet)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    const std::string value = "value";
    for (auto _ : state)
    {
        kv->put(key, value);
        benchmark::DoNotOptimize(kv->get(key));
    }
}

BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentPutOverwrite)(benchmark::State &state)
{
    const std::string key = "shared_key";
    const std::string value = "value";
    for (auto _ : state)
    {
        kv->put(key, value);
    }
}

BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentGetAfterDelete)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    // Delete key once before benchmark loop
    kv->deleteKey(key);
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(kv->get(key));
    }
}

BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentPutDelete)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    const std::string value = "value";
    for (auto _ : state)
    {
        kv->put(key, value);
        kv->deleteKey(key);
    }
}

BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentPut)->ThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentGet)->ThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentDeleteKey)->ThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentPutGet)->ThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentPutOverwrite)->ThreadRange(1, 4)->UseRealTime();
BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentPutDelete)->ThreadRange(1, 4)->UseRealTime();