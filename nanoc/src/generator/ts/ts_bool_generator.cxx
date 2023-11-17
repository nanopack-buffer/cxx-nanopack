#include "ts_bool_generator.hxx"

#include "../../string_util/case_conv.hxx"

std::string
TsBoolGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "boolean";
}

std::string
TsBoolGenerator::get_read_size_expression(NanoPack::DataType *data_type,
										  const std::string &var_name) {
	return "1";
}

void TsBoolGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	// clang-format off
	output.stream()
	<< snake_to_camel(field.field_name) << ": boolean";
	// clang-format on
}

void TsBoolGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "this." << field_name_camel_case << " = " << field_name_camel_case << ";" << std::endl;
	// clang-format on
}

void TsBoolGenerator::generate_field_declaration(CodeOutput &output,
												 const MessageField &field) {
	// clang-format off
	output.stream() << "public " << snake_to_camel(field.field_name) << ": boolean;" << std::endl;
	// clang-format on
}

void TsBoolGenerator::generate_read_code(CodeOutput &output,
										 NanoPack::DataType *type,
										 const std::string &var_name) {
	output.stream() << (output.is_variable_in_scope(var_name) ? "" : "const ")
					<< var_name << " = reader.readBoolean(ptr++);" << std::endl;
}

void TsBoolGenerator::generate_read_code(CodeOutput &output,
										 const MessageField &field) {
	generate_read_code(output, field.type.get(),
					   snake_to_camel(field.field_name));
}

void TsBoolGenerator::generate_write_code(CodeOutput &output,
										  NanoPack::DataType *type,
										  const std::string &var_name) {
	output.stream() << "writer.appendBoolean(" << var_name << ");" << std::endl;
}

void TsBoolGenerator::generate_write_code(CodeOutput &output,
										  const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "writer.appendBoolean(this." << field_name_camel_case << ");" << std::endl
	<< "writer.writeFieldSize(" << field.field_number << ", 1);" << std::endl;
	// clang-format on
}
