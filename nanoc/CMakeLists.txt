cmake_minimum_required(VERSION 3.25.2)

find_package(argparse REQUIRED)
find_package(yaml REQUIRED)
find_package(TBB REQUIRED)

add_executable(nanoc
        src/main.cxx
        src/parser/parse_schema.cxx
        src/generator/cxx_generator.cxx
        src/string_util/case_conv.cxx)

target_link_libraries(nanoc argparse::argparse yaml onetbb::onetbb)
