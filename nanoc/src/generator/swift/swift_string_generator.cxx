#include "swift_string_generator.hxx"

std::string
SwiftStringGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return "String";
}

std::string
SwiftStringGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											   const std::string &var_name) {
	return var_name + ".lengthOfBytes(using: .utf8)";
}

void SwiftStringGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << field.field_name << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftStringGenerator::generate_read_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "let " << var_name << "Size = buf.readUnalignedSize(at: ptr)" << std::endl
	<< "let " << var_name << ": " << get_type_declaration(type) << " = buf.read(at: ptr, withLength: " << var_name << "Size)" << std::endl
	<< "ptr += " << var_name << "Size";
	// clang-format on
}

void SwiftStringGenerator::generate_read_code(CodeOutput &output,
											  const MessageField &field) {
	// clang-format off
	output.stream()
	<< "let " << field.field_name << "Size = data.readSize(ofField: " << field.field_number << ")" << std::endl
	<< "guard let " << field.field_name << " = data.read(at: ptr, withLength: " << field.field_name << "Size) else {" << std::endl
	<< "  return nil" << std::endl
	<< "}" << std::endl
	<< "ptr += " << field.field_name << "Size" << std::endl
	<< "self." << field.field_name << " = " << field.field_name << std::endl
	<< std::endl;
	// clang-format on
}

void SwiftStringGenerator::generate_write_code(CodeOutput &output,
											   NanoPack::DataType *type,
											   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "buf.append(size: " << get_read_size_expression(type, var_name) << ")" << std::endl
	<< "buf.append(string: " << var_name << ")" << std::endl
	<< std::endl;
	// clang-format on
}

void SwiftStringGenerator::generate_write_code(CodeOutput &output,
											   const MessageField &field) {
	// clang-format off
	output.stream()
	<< "buf.write(size: " << get_read_size_expression(field.type.get(), field.field_name) << ", forField: " << field.field_number << ")" << std::endl
	<< "buf.append(string: " << field.field_name << ")" << std::endl
	<< std::endl;
	// clang-format on
}
