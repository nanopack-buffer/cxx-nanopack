cmake_minimum_required(VERSION 3.25.2)

project(nanopack
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(nanoc)

add_library(nanopack
        src/nanobuf/nanobuf.cxx)

# include example targets
add_subdirectory(examples/simple)

target_include_directories(nanopack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
