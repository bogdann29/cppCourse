cmake_minimum_required(VERSION 3.22)
project(Format)

set(CMAKE_CXX_STANDARD 20)

include_directories(src)
add_subdirectory(src)
enable_testing()
add_subdirectory(tests)

set(SOURCE_FILES main.cpp)
add_executable(main ${SOURCE_FILES})

target_link_libraries(main format_lib)