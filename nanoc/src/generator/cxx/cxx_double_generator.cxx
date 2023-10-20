#include "cxx_double_generator.hxx"

void CxxDoubleGenerator::generate_field_declaration(std::ostream &out,
													const MessageField &field) {
	out << "double " << field.field_name << ";" << std::endl;
}

void CxxDoubleGenerator::generate_read_code(std::ostream &out,
											const std::string &var_name) {
	// clang-format off
	out
	<< "  const double " << var_name << " = buf.read_double(ptr);" << std::endl
	<< "  ptr += sizeof(double);" << std::endl;
	// clang-format on
}

void CxxDoubleGenerator::generate_read_code(std::ostream &out,
											const MessageField &field) {
	generate_read_code(out, field.field_name);
	// clang-format off
	out
	// store the double value to the field
	<< " this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxDoubleGenerator::generate_write_field_code(std::ostream &out,
												   const MessageField &field) {
	// clang-format off
	out
	// write the size of double to the size header
	<< "  buf.write_field_size(" << field.field_number << ", 1);" << std::endl
	// append the double value to the end of the buffer
	<< "  buf.append_double(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
