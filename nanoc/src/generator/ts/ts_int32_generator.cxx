#include "ts_int32_generator.hxx"

#include "../../string_util/case_conv.hxx"

std::string
TsInt32Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "number";
}

std::string
TsInt32Generator::get_read_size_expression(NanoPack::DataType *data_type,
										   const std::string &var_name) {
	return "4";
}

void TsInt32Generator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	// clang-format off
	output.stream()
	<< snake_to_camel(field.field_name) << ": number";
	// clang-format on
}

void TsInt32Generator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "this." << field_name_camel_case << " = " << field_name_camel_case;
	// clang-format on
}

void TsInt32Generator::generate_field_declaration(CodeOutput &output,
												  const MessageField &field) {
	// clang-format off
	output.stream()
	<< "public " << snake_to_camel(field.field_name) << ": number;";
	// clang-format on
}

void TsInt32Generator::generate_read_code(CodeOutput &output,
										  NanoPack::DataType *type,
										  const std::string &var_name) {
	// clang-format off
	output.stream()
	<< (output.is_variable_in_scope(var_name) ? "" : "const ") << var_name << " = reader.readInt32(ptr);" << std::endl
	<< "ptr += 4;" << std::endl;
	// clang-format on
}

void TsInt32Generator::generate_read_code(CodeOutput &output,
										  const MessageField &field) {
	generate_read_code(output, field.type.get(),
					   snake_to_camel(field.field_name));
}

void TsInt32Generator::generate_write_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	output.stream() << "writer.appendInt32(" << var_name << ");" << std::endl;
}

void TsInt32Generator::generate_write_code(CodeOutput &output,
										   const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "writer.appendInt8(this." << field_name_camel_case << ");" << std::endl
	<< "writer.writeFieldSize(" << field.field_number << ", 4);" << std::endl;
	// clang-format on
}
