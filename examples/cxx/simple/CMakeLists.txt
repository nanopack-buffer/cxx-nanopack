cmake_minimum_required(VERSION 3.25.2)

add_executable(simple_example
        main.cxx
        person.np.cxx)

target_link_libraries(simple_example PUBLIC nanopack)
