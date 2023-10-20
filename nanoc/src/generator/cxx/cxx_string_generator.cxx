#include "cxx_string_generator.hxx"

void CxxStringGenerator::generate_field_declaration(std::ostream &out,
													const MessageField &field) {
	out << "std::string " << field.field_name << ";" << std::endl;
}

void CxxStringGenerator::generate_read_code(std::ostream &out,
											const std::string &var_name) {
	// clang-format off
	out
	// read string size from current buffer read ptr
	<< "  const int32_t " << var_name << "_size = buf.read_int32(ptr);" << std::endl
	// move read ptr
	<< "  ptr += sizeof(int32_t);" << std::endl
	// read string from buffer into var_name
	<< "  std::string " << var_name << " = buf.read_string(ptr, " << var_name << "_size);" << std::endl
	// move read ptr
	<< "  ptr += " << var_name << "_size;" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_read_code(std::ostream &out,
											const MessageField &field) {
	// clang-format off
	out
	// read string size from size header
	<< "  const int32_t " << field.field_name << "_size = buf.read_field_size(" << field.field_number << ");" << std::endl
	// read string from buffer into var_name
	<< "  std::string " << field.field_name << " = buf.read_string(ptr, " << field.field_name << "_size);" << std::endl
	// move read ptr
	<< "  ptr += " << field.field_name << "_size;" << std::endl
	// store value to field
	<< "  this->" << field.field_name << " = " << field.field_name << ";" << std::endl;
	// clang-format on
}

void CxxStringGenerator::generate_write_field_code(std::ostream &out,
												   const MessageField &field) {
	// clang-format off
	out
	// write the size of string stored in the field to the size header
	<< "  buf.write_field_size(" << field.field_number << ", " << field.field_name << ".size());" << std::endl
	// append the string data to the end of buffer
	<< "  buf.append_string(" << field.field_name << ");" << std::endl;
	// clang-format on
}
