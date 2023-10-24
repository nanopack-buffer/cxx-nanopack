cmake_minimum_required(VERSION 3.25.2)

find_package(argparse REQUIRED)
find_package(yaml REQUIRED)
find_package(TBB REQUIRED)

add_executable(nanoc
        src/main.cxx
        src/parser/parse_schema.cxx
        src/generator/cxx/cxx_generator.cxx
        src/string_util/case_conv.cxx
        src/data_type/np_string.cxx
        src/data_type/np_bool.cxx
        src/data_type/np_int8.cxx
        src/data_type/np_int32.cxx
        src/data_type/np_double.cxx
        src/generator/cxx/cxx_bool_generator.cxx
        src/generator/cxx/cxx_string_generator.cxx
        src/generator/cxx/cxx_int8_generator.cxx
        src/generator/cxx/cxx_int32_generator.cxx
        src/generator/cxx/cxx_double_generator.cxx
        src/message_schema.cxx
        src/data_type/type_factory.cxx
        src/data_type/np_array.cxx
        src/generator/cxx/cxx_array_generator.cxx
        src/generator/data_type_code_generator.cxx
        src/generator/code_output.cxx
)

target_link_libraries(nanoc argparse::argparse yaml onetbb::onetbb)
