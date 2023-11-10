#include "cxx_message_generator.hxx"

CxxMessageGenerator::CxxMessageGenerator() {}

std::string
CxxMessageGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return data_type->identifier();
}

std::string
CxxMessageGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											  const std::string &var_name) {
	return var_name + "_data.size()";
}

void CxxMessageGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	std::string message_type_name = field.type->identifier();
	bool is_self_referencing =
		message_type_name == output.get_message_schema().message_name;
	std::string type_declaration =
		is_self_referencing ? "std::shared_ptr<" + message_type_name + ">"
							: message_type_name;
	output.stream() << type_declaration << " " << field.field_name << ";";
}

void CxxMessageGenerator::generate_read_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	output.stream() << "int " << var_name << "_bytes_read;";
	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream() << var_name << " = " << type->identifier() << "(begin + ptr, " << var_name << "_bytes_read);" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream() << type->identifier() << " " << var_name << "(begin + ptr, " << var_name << "_bytes_read);" << std::endl;
		// clang-format on
	}
	output.stream() << "ptr += " << var_name << "_bytes_read;" << std::endl;
}

void CxxMessageGenerator::generate_read_code(CodeOutput &output,
											 const MessageField &field) {
	std::string message_type_name = field.type->identifier();
	bool is_self_referencing =
		message_type_name == output.get_message_schema().message_name;
	if (is_self_referencing) {
		// clang-format off
		output.stream()
		<< "if (reader.read_field_size(" << field.field_number << ") < 0) {" << std::endl
		<< "    " << field.field_name << " = nullptr;"
		<< "} else {" << std::endl
		<< "    int " << field.field_name << "_bytes_read = 0;"
		<< "    " << field.field_name << " = std::make_shared<" << message_type_name << ">(begin + ptr, " << field.field_name << "_bytes_read);" << std::endl
		<< "    ptr += " << field.field_name << "_bytes_read;" << std::endl
		<< "}" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< "int " << field.field_name << "_bytes_read = 0;" << std::endl
		<< field.field_name << " = " << message_type_name << "(begin + ptr, " << field.field_name << "_bytes_read);" << std::endl
		<< "ptr += " << field.field_name << "_bytes_read;" << std::endl;
		// clang-format on
	}
}

void CxxMessageGenerator::generate_write_code(CodeOutput &output,
											  NanoPack::DataType *type,
											  const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "const std::vector<uint8_t> " << var_name << "_data = " << var_name << ".data();" << std::endl
	<< "writer.append_bytes(" << var_name << "_data);" << std::endl;
	// clang-format on
}

void CxxMessageGenerator::generate_write_code(CodeOutput &output,
											  const MessageField &field) {
	std::string message_type_name = field.type->identifier();
	bool is_self_referencing =
		message_type_name == output.get_message_schema().message_name;

	if (is_self_referencing) {
		// clang-format off
		output.stream()
		<< "if (" << field.field_name << " != nullptr) {" << std::endl
		<< "    const std::vector<uint8_t> " << field.field_name << "_data = " << field.field_name << "->data();" << std::endl
		<< "    writer.append_bytes(" << field.field_name << "_data);" << std::endl
		<< "    writer.write_field_size(" << field.field_number << ", " << field.field_name << "_data.size());" << std::endl
		<< "} else {" << std::endl
		<< "    writer.write_field_size(" << field.field_number << ", -1);" << std::endl
		<< "}" << std::endl;
		// clang-format on
	} else {
		generate_write_code(output, field.type.get(), field.field_name);
		// clang-format off
		output.stream()
		<< "writer.write_field_size(" << field.field_number << ", " << field.field_name << "_data.size());";
		// clang-format on
	}
}
