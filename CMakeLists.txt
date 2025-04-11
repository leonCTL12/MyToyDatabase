cmake_minimum_required(VERSION 3.13)
project(MyToyDatabase LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Delete output folder if it exists and create a fresh one
file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/output)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)
# this is for static library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

#key value store library
file(GLOB_RECURSE KEY_VALUE_STORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/key_value_store_internal/*.cpp")
add_library(kvstore STATIC ${KEY_VALUE_STORE_SOURCES})
target_include_directories(kvstore PUBLIC ${CMAKE_SOURCE_DIR}/src/key_value_store_internal)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS 
    "${CMAKE_SOURCE_DIR}/src/app/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
)
add_executable(main ${APP_SOURCES})
target_link_libraries(main PRIVATE kvstore)

target_compile_options(main PRIVATE -g)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(main PRIVATE -fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(main PRIVATE -fdiagnostics-color=always)
endif()