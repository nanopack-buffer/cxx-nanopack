#include "swift_int32_generator.hxx"
#include "../../string_util/case_conv.hxx"

std::string
SwiftInt32Generator::get_type_declaration(NanoPack::DataType *data_type) {
	return "Int32";
}

std::string
SwiftInt32Generator::get_read_size_expression(NanoPack::DataType *data_type,
											  const std::string &var_name) {
	return "MemoryLayout<Int32>.size";
}

void SwiftInt32Generator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": Int32";
}

void SwiftInt32Generator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "self." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void SwiftInt32Generator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftInt32Generator::generate_read_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	if (output.is_variable_in_scope(var_name)) {
		output.stream() << var_name << " = data.readUnaligned(at: ptr)"
						<< std::endl;
	} else {
		// clang-format off
		output.stream()
		<< "let " << var_name << ": " << get_type_declaration(type) << " = data.readUnaligned(at: ptr)" << std::endl;
		// clang-format on
	}
	output.stream() << "ptr += 4" << std::endl;
}

void SwiftInt32Generator::generate_read_code(CodeOutput &output,
											 const MessageField &field) {
	const auto field_name_camel = snake_to_camel(field.field_name);
	generate_read_code(output, field.type.get(), field_name_camel);
}

void SwiftInt32Generator::generate_write_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	output.stream() << "data.append(int: " << var_name << ")"
					<< std::endl;
}

void SwiftInt32Generator::generate_write_code(CodeOutput &output,
											  const MessageField &field) {
	// clang-format off
	output.stream()
	<< "data.write(size: 4, ofField: " << field.field_number << ")" << std::endl;
	// clang-format on
	generate_write_code(output, field.type.get(),
						snake_to_camel(field.field_name));
}
