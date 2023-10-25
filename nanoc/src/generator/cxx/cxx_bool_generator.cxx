#include "cxx_bool_generator.hxx"

std::string
CxxBoolGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "std::string";
}

std::string
CxxBoolGenerator::get_read_size_expression(NanoPack::DataType *data_type,
										   const std::string &var_name) {
	return "sizeof(bool)";
}

void CxxBoolGenerator::generate_field_declaration(CodeOutput &output,
												  const MessageField &field) {
	output.stream() << get_type_declaration(nullptr) << " " << field.field_name
					<< ";" << std::endl;
}

void CxxBoolGenerator::generate_read_code(CodeOutput &output,
										  NanoPack::DataType *type,
										  const std::string &var_name) {
	// clang-format off
	output.stream()
	// read boolean value from current buffer read ptr, then move the read ptr
	<< "const " << get_type_declaration(nullptr) << " " << var_name << " = buf.read_bool(ptr++);" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxBoolGenerator::generate_read_code(CodeOutput &output,
										  const MessageField &field) {
	generate_read_code(output, nullptr, field.field_name);
	// clang-format off
	output.stream()
	<< "this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxBoolGenerator::generate_write_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "buf.append_bool(" << var_name << ");" << std::endl;
	// clang-format on
}

void CxxBoolGenerator::generate_write_code(CodeOutput &output,
										   const MessageField &field) {
	// clang-format off
	output.stream()
	// write the size of the boolean to the size header
	<< "buf.write_field_size(" << field.field_number << ", 1);" << std::endl
	// append the boolean value to the end of the buffer
	<< "buf.append_bool(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
