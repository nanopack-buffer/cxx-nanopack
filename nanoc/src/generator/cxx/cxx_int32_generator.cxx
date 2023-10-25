#include "cxx_int32_generator.hxx"

std::string
CxxInt32Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "int32_t";
}

std::string
CxxInt32Generator::get_read_size_expression(NanoPack::DataType *data_type,
											const std::string &var_name) {
	return "sizeof(int32_t)";
}

void CxxInt32Generator::generate_field_declaration(CodeOutput &output,
												   const MessageField &field) {
	output.stream() << get_type_declaration(nullptr) << " " << field.field_name
					<< ";" << std::endl;
}

void CxxInt32Generator::generate_read_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "const " << get_type_declaration(nullptr) << " " << var_name << " = buf.read_int32(ptr);" << std::endl
	<< "ptr += sizeof(int32_t);" << std::endl;
	// clang-format on
}

void CxxInt32Generator::generate_read_code(CodeOutput &output,
										   const MessageField &field) {
	generate_read_code(output, nullptr, field.field_name);
	// clang-format off
	output.stream()
	// store the int32 value to the field
	<< "this->" << field.field_name << " = " << field.field_name << ";" << std::endl
	<< std::endl;
	// clang-format on
}

void CxxInt32Generator::generate_write_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "buf.append_int32(" << var_name << ");" << std::endl;
	// clang-format on
}

void CxxInt32Generator::generate_write_code(CodeOutput &output,
											const MessageField &field) {
	// clang-format off
	output.stream()
	// write the size of int32 to the size header
	<< "buf.write_field_size(" << field.field_number << ", sizeof(int32_t));" << std::endl
	// append the int32 value to the end of the buffer
	<< "buf.append_int32(" << field.field_name << ");" << std::endl
	<< std::endl;
	// clang-format on
}
