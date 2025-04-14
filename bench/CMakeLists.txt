# Define the benchmark executable
add_executable(all_benchmarks key_value_store_bench.cpp multithread_key_value_store_bench.cpp)

# Link against kvstore and Google Benchmark
# benchmark_main provides the main function for Google Benchmark
target_link_libraries(all_benchmarks PRIVATE kvstore benchmark::benchmark benchmark_main)

# Ensure benchmarks are optimized for realistic performance
target_compile_options(all_benchmarks PRIVATE -O2)