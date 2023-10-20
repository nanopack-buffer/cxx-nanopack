#include "cxx_int32_generator.hxx"

void CxxInt32Generator::generate_field_declaration(std::ostream &out,
												   const MessageField &field) {
	out << "int32_t " << field.field_name << ";" << std::endl;
}

void CxxInt32Generator::generate_read_code(std::ostream &out,
										   const std::string &var_name) {
	// clang-format off
	out
	<< "  const int32_t " << var_name << " = buf.read_int32(ptr);" << std::endl
	<< "  ptr += sizeof(int32_t);" << std::endl;
	// clang-format on
}

void CxxInt32Generator::generate_read_code(std::ostream &out,
										   const MessageField &field) {
	generate_read_code(out, field.field_name);
	// clang-format off
	out
	// store the int32 value to the field
	<< " this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxInt32Generator::generate_write_field_code(std::ostream &out,
												  const MessageField &field) {
	// clang-format off
	out
	// write the size of int32 to the size header
	<< "  buf.write_field_size(" << field.field_number << ", 1);" << std::endl
	// append the int32 value to the end of the buffer
	<< "  buf.append_int32(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
