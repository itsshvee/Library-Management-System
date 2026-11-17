cmake_minimum_required(VERSION 3.15)
project(LibraryManagementBackend VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

find_package(Drogon CONFIG REQUIRED)

include_directories(include)

set(BACKEND_SOURCES
    src/main.cc
    src/BookController.cc
    src/TransactionController.cc
    src/DashboardController.cc
)

add_executable(library_backend ${BACKEND_SOURCES})
target_link_libraries(library_backend PRIVATE Drogon::Drogon)

# Copy config.json to build directory
add_custom_command(TARGET library_backend POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/config.json
    $<TARGET_FILE_DIR:library_backend>/config.json
)

