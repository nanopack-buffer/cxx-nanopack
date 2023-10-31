#include "swift_int8_generator.hxx"

std::string
SwiftInt8Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "Int8";
}

std::string
SwiftInt8Generator::get_read_size_expression(NanoPack::DataType *data_type,
											 const std::string &var_name) {
	return "MemoryLayout<Int8>.size";
}

void SwiftInt8Generator::generate_field_declaration(CodeOutput &output,
													const MessageField &field) {
	output.stream() << "let " << field.field_name << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftInt8Generator::generate_read_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "let " << var_name << ": " << get_type_declaration(type) << " = buf.read(at: ptr)" << std::endl
	<< "ptr += 1" << std::endl;
	// clang-format on
}

void SwiftInt8Generator::generate_read_code(CodeOutput &output,
											const MessageField &field) {
	generate_read_code(output, field.type.get(), field.field_name);
	output.stream() << "self." << field.field_name << " = " << field.field_name
					<< std::endl
					<< std::endl;
}

void SwiftInt8Generator::generate_write_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	output.stream() << "buf.append(int: " << var_name << ")" << std::endl;
}

void SwiftInt8Generator::generate_write_code(CodeOutput &output,
											 const MessageField &field) {
	// clang-format off
	output.stream()
	<< "buf.write(size: 1, forField: " << field.field_number << ")" << std::endl;
	// clang-format on
	generate_write_code(output, field.type.get(), field.field_name);
}
