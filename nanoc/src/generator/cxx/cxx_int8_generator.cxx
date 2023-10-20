#include "cxx_int8_generator.hxx"

void CxxInt8Generator::generate_field_declaration(std::ostream &out,
												  const MessageField &field) {
	out << "int8_t " << field.field_name << ";" << std::endl;
}

void CxxInt8Generator::generate_read_code(std::ostream &out,
										  const std::string &var_name) {
	// clang-format off
	// read the int8 value from the current buffer read ptr, then move the read ptr
	out << "  const bool " << var_name << " = buf.read_int8(ptr++);" << std::endl;
	// clang-format on
}

void CxxInt8Generator::generate_read_code(std::ostream &out,
										  const MessageField &field) {
	generate_read_code(out, field.field_name);
	// clang-format off
	out
	// store the int8 value to the field
	<< " this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxInt8Generator::generate_write_field_code(std::ostream &out,
												 const MessageField &field) {
	// clang-format off
	out
	// write the size of int8 to the size header
	<< "  buf.write_field_size(" << field.field_number << ", 1);" << std::endl
	// append the int8 value to the end of the buffer
	<< "  buf.append_int8(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
