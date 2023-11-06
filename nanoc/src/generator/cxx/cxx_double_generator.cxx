#include "cxx_double_generator.hxx"

std::string
CxxDoubleGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "double";
}

std::string
CxxDoubleGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											 const std::string &var_name) {
	return "sizeof(double)";
}

void CxxDoubleGenerator::generate_field_declaration(CodeOutput &output,
													const MessageField &field) {
	output.stream() << get_type_declaration(nullptr) << " " << field.field_name
					<< ";" << std::endl;
}

void CxxDoubleGenerator::generate_read_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< var_name << " = buf.read_double(ptr);" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< "const " << get_type_declaration(nullptr) << " " << var_name << " = buf.read_double(ptr);" << std::endl;
		// clang-format on
	}
	output.stream() << "ptr += sizeof(double);" << std::endl << std::endl;
}

void CxxDoubleGenerator::generate_read_code(CodeOutput &output,
											const MessageField &field) {
	generate_read_code(output, nullptr, field.field_name);
	// clang-format off
	output.stream()
	// store the double value to the field
	<< "this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxDoubleGenerator::generate_write_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "buf.append_double(" << var_name << ");" << std::endl;
	// clang-format on
}

void CxxDoubleGenerator::generate_write_code(CodeOutput &output,
											 const MessageField &field) {
	// clang-format off
	output.stream()
	// write the size of double to the size header
	<< "buf.write_field_size(" << field.field_number << ", sizeof(double));" << std::endl
	// append the double value to the end of the buffer
	<< "  buf.append_double(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
