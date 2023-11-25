cmake_minimum_required(VERSION 3.25.2)

add_executable(inheritance_example
        main.cxx
        text.np.cxx
        widget.np.cxx
        nanopack_message_factory.cxx)

target_link_libraries(inheritance_example PUBLIC nanopack)
