cmake_minimum_required(VERSION 3.25.2)

find_package(rapidjson)

add_executable(benchmark
        main.cxx
        person.np.cxx)

target_link_libraries(benchmark PUBLIC nanopack rapidjson)
