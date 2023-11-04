#include "swift_int32_generator.hxx"
#include "../../string_util/case_conv.hxx"

std::string
SwiftInt32Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "Int32";
}

std::string
SwiftInt32Generator::get_read_size_expression(NanoPack::DataType *data_type,
											  const std::string &var_name) {
	return "MemoryLayout<Int32>.size";
}

void SwiftInt32Generator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftInt32Generator::generate_read_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "let " << var_name << ": " << get_type_declaration(type) << " = data.readUnaligned(at: ptr)" << std::endl
	<< "ptr += 4" << std::endl;
	// clang-format on
}

void SwiftInt32Generator::generate_read_code(CodeOutput &output,
											 const MessageField &field) {
	const auto field_name_camel = snake_to_camel(field.field_name);
	generate_read_code(output, field.type.get(), field_name_camel);
	output.stream() << "self." << field_name_camel << " = " << field_name_camel
					<< std::endl
					<< std::endl;
}

void SwiftInt32Generator::generate_write_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	output.stream() << "data.append(int: " << var_name << ".littleEndian)"
					<< std::endl;
}

void SwiftInt32Generator::generate_write_code(CodeOutput &output,
											  const MessageField &field) {
	// clang-format off
	output.stream()
	<< "data.write(size: 4, ofField: " << field.field_number << ")" << std::endl;
	// clang-format on
	generate_write_code(output, field.type.get(),
						snake_to_camel(field.field_name));
}
