cmake_minimum_required(VERSION 3.15)
project(LibraryManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

include_directories(include)

set(COMMON_SOURCES
    src/Book.cpp
    src/Transaction.cpp
    src/FileManager.cpp
    src/Library.cpp
)

# Main executable
add_executable(LibraryManagementSystem src/main.cpp ${COMMON_SOURCES})

# Test runner executable
add_executable(test_runner tests/test_runner.cpp ${COMMON_SOURCES})

