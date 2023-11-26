#include "swift_string_generator.hxx"
#include "../../string_util/case_conv.hxx"

std::string
SwiftStringGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "String";
}

std::string
SwiftStringGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											   const std::string &var_name) {
	return var_name + ".lengthOfBytes(using: .utf8)";
}

void SwiftStringGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": String";
}

void SwiftStringGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "self." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void SwiftStringGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftStringGenerator::generate_read_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "let " << var_name << "Size = data.readUnalignedSize(at: ptr)" << std::endl
	<< "ptr += 4" << std::endl
	<< "guard let " << var_name << " = data.read(at: ptr, withLength: " << var_name << "Size) else {" << std::endl
	<< "  return nil" << std::endl
	<< "}" << std::endl
	<< "ptr += " << var_name << "Size" << std::endl;
	// clang-format on
}

void SwiftStringGenerator::generate_read_code(CodeOutput &output,
											  const MessageField &field) {
	const auto field_name_camel = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "let " << field_name_camel << "Size = data.readSize(ofField: " << field.field_number << ")" << std::endl
	<< "guard let " << field_name_camel << " = data.read(at: ptr, withLength: " << field_name_camel << "Size) else {" << std::endl
	<< "  return nil" << std::endl
	<< "}" << std::endl
	<< "ptr += " << field_name_camel << "Size" << std::endl
	<< std::endl;
	// clang-format on
}

void SwiftStringGenerator::generate_write_code(CodeOutput &output,
											   NanoPack::DataType *type,
											   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "data.append(size: " << get_read_size_expression(type, var_name) << ")" << std::endl
	<< "data.append(string: " << var_name << ")" << std::endl
	<< std::endl;
	// clang-format on
}

void SwiftStringGenerator::generate_write_code(CodeOutput &output,
											   const MessageField &field) {
	const auto field_name_camel = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "data.write(size: " << get_read_size_expression(field.type.get(), snake_to_camel(field.field_name)) << ", ofField: " << field.field_number << ")" << std::endl
	<< "data.append(string: " << snake_to_camel(field.field_name) << ")" << std::endl
	<< std::endl;
	// clang-format on
}
