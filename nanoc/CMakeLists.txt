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
        src/parser/type_factory.cxx
        src/data_type/np_array.cxx
        src/generator/cxx/cxx_array_generator.cxx
        src/generator/data_type_code_generator.cxx
        src/generator/code_output.cxx
        src/data_type/np_map.cxx
        src/generator/cxx/cxx_map_generator.cxx
        src/generator/swift/swift_generator.cxx
        src/generator/swift/swift_bool_generator.cxx
        src/generator/swift/swift_int8_generator.cxx
        src/generator/swift/swift_int32_generator.cxx
        src/generator/swift/swift_string_generator.cxx
        src/generator/swift/swift_string_generator.hxx
        src/generator/swift/swift_double_generator.cxx
        src/generator/swift/swift_double_generator.hxx
        src/generator/swift/swift_array_generator.cxx
        src/generator/swift/swift_array_generator.hxx
        src/generator/swift/swift_map_generator.cxx
        src/generator/swift/swift_map_generator.hxx
        src/data_type/np_optional.cxx
        src/data_type/np_optional.hxx
        src/generator/cxx/cxx_optional_generator.cxx
        src/generator/cxx/cxx_optional_generator.hxx
        src/generator/swift/swift_optional_generator.cxx
        src/generator/swift/swift_optional_generator.hxx
        src/data_type/np_message.cxx
        src/data_type/np_message.hxx
        src/resolver/resolve_schemas.cxx
        src/resolver/resolve_schemas.hxx
        src/generator/cxx/cxx_message_generator.cxx
        src/generator/cxx/cxx_message_generator.hxx
        src/generator/swift/swift_message_generator.cxx
        src/generator/swift/swift_message_generator.hxx
        src/generator/ts/ts_generator.cxx
        src/generator/ts/ts_generator.hxx
        src/generator/ts/ts_int8_generator.cxx
        src/generator/ts/ts_int8_generator.hxx
        src/generator/ts/ts_int32_generator.cxx
        src/generator/ts/ts_int32_generator.hxx
        src/generator/ts/ts_bool_generator.cxx
        src/generator/ts/ts_bool_generator.hxx
        src/generator/ts/ts_string_generator.cxx
        src/generator/ts/ts_string_generator.hxx
        src/generator/ts/ts_array_generator.cxx
        src/generator/ts/ts_array_generator.hxx
        src/generator/ts/ts_map_generator.cxx
        src/generator/ts/ts_map_generator.hxx
        src/generator/ts/ts_optional_generator.cxx
        src/generator/ts/ts_optional_generator.hxx
        src/generator/ts/ts_message_generator.cxx
        src/generator/ts/ts_message_generator.hxx
)

target_link_libraries(nanoc argparse::argparse yaml onetbb::onetbb)
