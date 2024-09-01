cmake_minimum_required(VERSION 3.15)

project(nanopack
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

option(BUILD_TESTING "" OFF)

add_library(nanopack
	include/nanopack/nanopack.hxx
	include/nanopack/message.hxx
	include/nanopack/reader.hxx
	include/nanopack/writer.hxx
	include/nanopack/any.hxx
	src/reader.cxx
	src/writer.cxx
	src/any.cxx
	src/rpc/in_memory_channel.cxx
	src/rpc/standard_io_channel.cxx
	src/rpc/rpc_client.cxx
	src/rpc/rpc_client_channel.cxx
	src/rpc/rpc_server.cxx
	src/rpc/rpc_server_channel.cxx)

target_include_directories(nanopack PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (BUILD_TESTING)
	add_executable(${PROJECT_NAME}_test test/test.cxx)
	target_include_directories(${PROJECT_NAME}_test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(${PROJECT_NAME}_test PRIVATE nanopack)
endif()

install(TARGETS nanopack
	DESTINATION lib)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/nanopack"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
