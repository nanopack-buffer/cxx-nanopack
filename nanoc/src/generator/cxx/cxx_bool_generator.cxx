#include "cxx_bool_generator.hxx"

void CxxBoolGenerator::generate_field_declaration(std::ostream &out,
												  const MessageField &field) {
	out << "bool " << field.field_name << ";" << std::endl;
}

void CxxBoolGenerator::generate_read_code(std::ostream &out,
										  const std::string &var_name) {
	// clang-format off
	out
	// read boolean value from current buffer read ptr, then move the read ptr
	<< "  const bool " << var_name << " = buf.read_bool(ptr++);" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxBoolGenerator::generate_read_code(std::ostream &out,
										  const MessageField &field) {
	generate_read_code(out, field.field_name);
	// clang-format off
	out
	<< "  this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxBoolGenerator::generate_write_field_code(std::ostream &out,
												 const MessageField &field) {
	// clang-format off
	out
	// write the size of the boolean to the size header
	<< "  buf.write_field_size(" << field.field_number << ", 1);" << std::endl
	// append the boolean value to the end of the buffer
	<< "  buf.append_bool(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
