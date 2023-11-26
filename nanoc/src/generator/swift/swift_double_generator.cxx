#include "swift_double_generator.hxx"
#include "../../string_util/case_conv.hxx"

std::string
SwiftDoubleGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "Double";
}
std::string
SwiftDoubleGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											   const std::string &var_name) {
	return "MemoryLayout<Double>.size";
}

void SwiftDoubleGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": Double";
}

void SwiftDoubleGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "self." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void SwiftDoubleGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftDoubleGenerator::generate_read_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< var_name << " = data.read(at: ptr)" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< "let " << var_name << ": " << get_type_declaration(type) << " = data.read(at: ptr)" << std::endl;
		// clang-format on
	}
	output.stream() << "ptr += 8" << std::endl;
}

void SwiftDoubleGenerator::generate_read_code(CodeOutput &output,
											  const MessageField &field) {
	const auto field_name_camel = snake_to_camel(field.field_name);
	generate_read_code(output, field.type.get(), field_name_camel);
}

void SwiftDoubleGenerator::generate_write_code(CodeOutput &output,
											   NanoPack::DataType *type,
											   const std::string &var_name) {
	output.stream() << "data.append(double: " << var_name << ")" << std::endl;
}

void SwiftDoubleGenerator::generate_write_code(CodeOutput &output,
											   const MessageField &field) {
	// clang-format off
	output.stream()
	<< "data.write(size: 8, ofField: " << field.field_number << ")" << std::endl
	<< "data.append(double: " << snake_to_camel(field.field_name) << ")" << std::endl
	<< std::endl;
	// clang-format on
}
