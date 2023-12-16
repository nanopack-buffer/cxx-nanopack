#include "ts_int64_generator.hxx"

#include "../../string_util/case_conv.hxx"

std::string
TsInt64Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "bigint";
}

std::string
TsInt64Generator::get_read_size_expression(NanoPack::DataType *data_type,
										   const std::string &var_name) {
	return "8";
}

void TsInt64Generator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	// clang-format off
	output.stream()
	<< snake_to_camel(field.field_name) << ": bigint";
	// clang-format on
}

void TsInt64Generator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "this." << field_name_camel_case << " = " << field_name_camel_case;
	// clang-format on
}

void TsInt64Generator::generate_field_declaration(CodeOutput &output,
												  const MessageField &field) {
	// clang-format off
	output.stream()
	<< "public " << snake_to_camel(field.field_name) << ": bigint;";
	// clang-format on
}

void TsInt64Generator::generate_read_code(CodeOutput &output,
										  NanoPack::DataType *type,
										  const std::string &var_name) {
	// clang-format off
	output.stream()
	<< (output.is_variable_in_scope(var_name) ? "" : "const ") << var_name << " = reader.readInt64(ptr);" << std::endl
	<< "ptr += 8;" << std::endl;
	// clang-format on
}

void TsInt64Generator::generate_read_code(CodeOutput &output,
										  const MessageField &field) {
	generate_read_code(output, field.type.get(),
					   snake_to_camel(field.field_name));
}

void TsInt64Generator::generate_write_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	output.stream() << "writer.appendInt64(" << var_name << ");" << std::endl;
}

void TsInt64Generator::generate_write_code(CodeOutput &output,
										   const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "writer.appendInt64(this." << field_name_camel_case << ");" << std::endl
	<< "writer.writeFieldSize(" << field.field_number << ", 8);" << std::endl;
	// clang-format on
}
