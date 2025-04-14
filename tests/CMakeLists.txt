# Define the test executable 
# The reason why I have combine all the tests into one executable is that for easy vscode debugging
add_executable(all_tests kv_store_test.cpp multithread_kv_store_test.cpp)
# Link against your library (e.g., logger) and Google Test
# gtestmain is the main function for Google Test
target_link_libraries(all_tests PRIVATE kvstore gtest gtest_main)

# Integrate with CTest for automatic test discovery
include(GoogleTest)

# Add the test executable to CTest, so that the command ctest can discover all the test in the executable all_tests
gtest_discover_tests(all_tests)