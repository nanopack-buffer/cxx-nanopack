#include "swift_bool_generator.hxx"
#include "../../string_util/case_conv.hxx"

std::string
SwiftBoolGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "Bool";
}

std::string
SwiftBoolGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											 const std::string &var_name) {
	return "MemoryLayout<Bool>.size";
}

void SwiftBoolGenerator::generate_field_declaration(CodeOutput &output,
													const MessageField &field) {
	output.stream() << "let " << field.field_name << ": "
					<< get_type_declaration(nullptr) << std::endl;
}

void SwiftBoolGenerator::generate_read_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "let " << snake_to_camel(var_name) << ": " << get_type_declaration(type) << " = data.read(at: ptr)" << std::endl
	<< "ptr += 1" << std::endl;
	// clang-format on
}

void SwiftBoolGenerator::generate_read_code(CodeOutput &output,
											const MessageField &field) {
	const auto field_name_camel = snake_to_camel(field.field_name);
	generate_read_code(output, field.type.get(), field_name_camel);
	output.stream() << "self." << field_name_camel << " = " << field_name_camel
					<< std::endl
					<< std::endl;
}

void SwiftBoolGenerator::generate_write_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	output.stream() << "data.append(bool: " << var_name << ")" << std::endl;
}

void SwiftBoolGenerator::generate_write_code(CodeOutput &output,
											 const MessageField &field) {
	// clang-format off
	output.stream()
	<< "data.write(size: 1, ofField: " << field.field_number << ")" << std::endl;
	// clang-format on
	generate_write_code(output, field.type.get(),
						snake_to_camel(field.field_name));
}
