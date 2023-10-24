#include "cxx_string_generator.hxx"

std::string
CxxStringGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "std::string";
}

std::string
CxxStringGenerator::get_read_size_expression(const std::string &var_name) {
	return var_name + ".size()";
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
	<< "const int32_t " << var_name << "_size = buf.read_int32(ptr);" << std::endl
	// move read ptr
	<< "ptr += sizeof(int32_t);" << std::endl
	// read string from buffer into var_name
	<< get_type_declaration(nullptr) << " " << var_name << " = buf.read_string(ptr, " << var_name << "_size);" << std::endl
	// move read ptr
	<< "ptr += " << var_name << "_size;" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_read_code(CodeOutput &output,
											const MessageField &field) {
	// clang-format off
	output.stream()
	// read string size from size header
	<< "const int32_t " << field.field_name << "_size = buf.read_field_size(" << field.field_number << ");" << std::endl
	// read string from buffer into var_name
	<< get_type_declaration(nullptr) << " " << field.field_name << " = buf.read_string(ptr, " << field.field_name << "_size);" << std::endl
	// move read ptr
	<< "ptr += " << field.field_name << "_size;" << std::endl
	// store value to field
	<< "this->" << field.field_name << " = " << field.field_name << ";" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_write_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "buf.append_int32(" << var_name << ".size());" << std::endl
	<< "buf.append_string(" << var_name << ");" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_write_code(CodeOutput &output,
											 const MessageField &field) {
	// clang-format off
	output.stream()
	// write the size of string stored in the field to the size header
	<< "buf.write_field_size(" << field.field_number << ", " << field.field_name << ".size());" << std::endl
	// append the string data to the end of buffer
	<< "buf.append_string(" << field.field_name << ");" << std::endl;
	// clang-format on
}
