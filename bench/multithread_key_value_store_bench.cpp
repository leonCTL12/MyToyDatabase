#include <benchmark/benchmark.h>
#include <mutex>

// Shared counter and mutex
static int counter = 0;
static std::mutex mutex;

// Multi-threaded benchmark: increment a shared counter
// Unlike unit test, we are defining what a thread should do and do multi-threading at the register statement below
static void BM_IncrementCounter(benchmark::State &state)
{
    for (auto _ : state)
    {
        std::lock_guard<std::mutex> lock(mutex);
        counter++;
    }
}

// Register the benchmark for multiple threads
// This make it run for 1,2,4,8 threads
BENCHMARK(BM_IncrementCounter)->ThreadRange(1, 8)->UseRealTime();