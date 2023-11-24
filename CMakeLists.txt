cmake_minimum_required(VERSION 3.25.2)

project(nanopack
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(nanoc)

add_library(nanopack
        include/nanopack/reader.hxx
        src/reader.cxx
        src/writer.cxx
        src/message_stream.cxx)

# include example targets
add_subdirectory(examples/cxx/simple)
add_subdirectory(examples/cxx/benchmark)

target_include_directories(nanopack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
