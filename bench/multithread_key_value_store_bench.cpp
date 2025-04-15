#include <benchmark/benchmark.h>
#include "key_value_store.h"

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
BENCHMARK_DEFINE_F(KeyValueStoreFixture, BM_ConcurrentGet)(benchmark::State &state)
{
    std::string key = "key" + std::to_string(state.thread_index());
    for (auto _ : state)
    {
        benchmark::DoNotOptimize(kv->get(key));
    }
}

BENCHMARK_REGISTER_F(KeyValueStoreFixture, BM_ConcurrentGet)->ThreadRange(1, 4)->UseRealTime();