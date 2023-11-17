#include "ts_string_generator.hxx"

#include "../../string_util/case_conv.hxx"

std::string
TsStringGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "string";
}

std::string
TsStringGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											const std::string &var_name) {
	return var_name + "ByteLength + 4";
}

void TsStringGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": string"
					<< std::endl;
}

void TsStringGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream() << "this." << field_name_camel_case << " = " << field_name_camel_case;
	// clang-format on
}

void TsStringGenerator::generate_field_declaration(CodeOutput &output,
												   const MessageField &field) {
	// clang-format off
	output.stream() << "public " << snake_to_camel(field.field_name) << ": string;" << std::endl;
	// clang-format on
}

void TsStringGenerator::generate_read_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "const " << var_name << "ByteLength = reader.readInt32(ptr);" << std::endl
	<< "ptr += 4" << std::endl
	<< (output.is_variable_in_scope(var_name) ? "" : "const ") << var_name << " = reader.readString(ptr, " << var_name << "ByteLength);" << std::endl
	<< "ptr += " << var_name << "ByteLength;" << std::endl;
	// clang-format on
}

void TsStringGenerator::generate_read_code(CodeOutput &output,
										   const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "const " << field_name_camel_case << "ByteLength = reader.readFieldSize(" << field.field_number << ");" << std::endl
	<< (output.is_variable_in_scope(field_name_camel_case) ? "" : "const ") << field_name_camel_case << " = reader.readString(ptr, " << field_name_camel_case << "ByteLength);" << std::endl
	<< "ptr += " << field_name_camel_case << "ByteLength;" << std::endl;
	// clang-format on
}

void TsStringGenerator::generate_write_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "const " << var_name << "ByteLength = writer.appendStringAndSize(" << var_name << ");" << std::endl;
	// clang-format on
}

void TsStringGenerator::generate_write_code(CodeOutput &output,
											const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "const " << field_name_camel_case << "ByteLength = writer.appendString(this." << field_name_camel_case << ");" << std::endl
	<< "writer.writeFieldSize(" << field.field_number << ", " << field_name_camel_case << "ByteLength);" << std::endl;
	// clang-format on
}
