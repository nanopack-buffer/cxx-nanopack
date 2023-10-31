#include "swift_double_generator.hxx"

std::string
SwiftDoubleGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "Double";
}
std::string
SwiftDoubleGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											   const std::string &var_name) {
	return "MemoryLayout<Double>.size";
}

void SwiftDoubleGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << field.field_name << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftDoubleGenerator::generate_read_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "let " << var_name << ": " << get_type_declaration(type) << " = buf.read(at: ptr)" << std::endl
	<< "ptr += 8" << std::endl;
	// clang-format on
}

void SwiftDoubleGenerator::generate_read_code(CodeOutput &output,
											  const MessageField &field) {
	generate_read_code(output, field.type.get(), field.field_name);
	output.stream() << "self." << field.field_name << " = " << field.field_name
					<< std::endl;
}

void SwiftDoubleGenerator::generate_write_code(CodeOutput &output,
											   NanoPack::DataType *type,
											   const std::string &var_name) {
	output.stream() << "buf.append(double: " << var_name << ")" << std::endl;
}

void SwiftDoubleGenerator::generate_write_code(CodeOutput &output,
											   const MessageField &field) {
	// clang-format off
	output.stream()
	<< "buf.write(size: 8, forField: " << field.field_number << ")" << std::endl
	<< "buf.append(double: " << field.field_name << ")" << std::endl
	<< std::endl;
	// clang-format on
}
