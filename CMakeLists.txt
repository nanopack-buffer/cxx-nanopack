cmake_minimum_required(VERSION 3.15)

project(nanopack
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

add_library(nanopack
				include/nanopack/nanopack.hxx
        include/nanopack/message.hxx
        include/nanopack/reader.hxx
        include/nanopack/writer.hxx
        include/nanopack/any.hxx
        src/reader.cxx
        src/writer.cxx
        src/any.cxx)

target_include_directories(nanopack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
