#include "cxx_string_generator.hxx"

std::string
CxxStringGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "std::string";
}

std::string
CxxStringGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											 const std::string &var_name) {
	return var_name + ".size() + sizeof(int32_t)";
}

void CxxStringGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "std::string " << field.field_name;
}

void CxxStringGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	output.stream() << field.field_name << "(std::move(" << field.field_name
					<< "))";
}

void CxxStringGenerator::generate_field_declaration(CodeOutput &output,
													const MessageField &field) {
	output.stream() << get_type_declaration(nullptr) << " " << field.field_name
					<< ";" << std::endl;
}

void CxxStringGenerator::generate_read_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	// clang-format off
	output.stream()
	// read string size from current buffer read ptr
	<< "const int32_t " << var_name << "_size = reader.read_int32(ptr);" << std::endl
	// move read ptr
	<< "ptr += sizeof(int32_t);" << std::endl
	// read string from buffer into var_name
	<< (output.is_variable_in_scope(var_name) ? "" : get_type_declaration(nullptr) + " ") << var_name << " = reader.read_string(ptr, " << var_name << "_size);" << std::endl
	// move read ptr
	<< "ptr += " << var_name << "_size;" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_read_code(CodeOutput &output,
											const MessageField &field) {
	// clang-format off
	output.stream()
	// read string size from size header
	<< "const int32_t " << field.field_name << "_size = reader.read_field_size(" << field.field_number << ");" << std::endl
	// read string from buffer into the field
	<< field.field_name << " = reader.read_string(ptr, " << field.field_name << "_size);" << std::endl
	// move read ptr
	<< "ptr += " << field.field_name << "_size;" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_write_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "writer.append_int32(" << var_name << ".size());" << std::endl
	<< "writer.append_string(" << var_name << ");" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_write_code(CodeOutput &output,
											 const MessageField &field) {
	// clang-format off
	output.stream()
	// write the size of string stored in the field to the size header
	<< "writer.write_field_size(" << field.field_number << ", " << field.field_name << ".size());" << std::endl
	// append the string data to the end of buffer
	<< "writer.append_string(" << field.field_name << ");" << std::endl;
	// clang-format on
}
