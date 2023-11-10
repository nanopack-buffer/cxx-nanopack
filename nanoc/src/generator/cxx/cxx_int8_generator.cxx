#include "cxx_int8_generator.hxx"

std::string
CxxInt8Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "int8_t";
}

std::string
CxxInt8Generator::get_read_size_expression(NanoPack::DataType *data_type,
										   const std::string &var_name) {
	return "sizeof(int8)";
}

void CxxInt8Generator::generate_field_declaration(CodeOutput &output,
												  const MessageField &field) {
	output.stream() << get_type_declaration(nullptr) << " " << field.field_name
					<< ";" << std::endl;
}

void CxxInt8Generator::generate_read_code(CodeOutput &output,
										  NanoPack::DataType *type,
										  const std::string &var_name) {
	if (output.is_variable_in_scope(var_name)) {
		output.stream() << var_name << " = reader.read_int8(ptr++);" << std::endl;
	} else {
		// clang-format off
		// read the int8 value from the current buffer read ptr, then move the read ptr
		output.stream()
		<< "const " << get_type_declaration(nullptr) << " " << var_name << " = reader.read_int8(ptr++);" << std::endl;
		// clang-format on
	}
}

void CxxInt8Generator::generate_read_code(CodeOutput &output,
										  const MessageField &field) {
	generate_read_code(output, nullptr, field.field_name);
	// clang-format off
	output.stream()
	// store the int8 value to the field
	<< "this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxInt8Generator::generate_write_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "writer.append_int8(" << var_name << ");" << std::endl;
	// clang-format on
}

void CxxInt8Generator::generate_write_code(CodeOutput &output,
										   const MessageField &field) {
	// clang-format off
	output.stream()
	// write the size of int8 to the size header
	<< "writer.write_field_size(" << field.field_number << ", 1);" << std::endl
	// append the int8 value to the end of the buffer
	<< "writer.append_int8(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
