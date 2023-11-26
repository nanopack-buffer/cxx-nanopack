#include "swift_message_generator.hxx"
#include "../../string_util/case_conv.hxx"

std::string
SwiftMessageGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return data_type->identifier();
}

std::string
SwiftMessageGenerator::get_read_size_expression(NanoPack::DataType *data_type,
												const std::string &var_name) {
	return var_name + "Data.count";
}

void SwiftMessageGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": "
					<< field.type->identifier();
}

void SwiftMessageGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "self." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void SwiftMessageGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< std::endl;
	const std::string message_type_name = field.type->identifier();
	const bool is_self_referencing =
		output.get_message_schema().message_name == message_type_name;
	if (is_self_referencing) {
		output.stream() << message_type_name << "?" << std::endl;
	} else {
		output.stream() << message_type_name << std::endl;
	}
}

void SwiftMessageGenerator::generate_read_code(CodeOutput &output,
											   NanoPack::DataType *type,
											   const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "var " << var_name << "Size = 0" << std::endl
	<< "guard let " << var_name << " = " << type->identifier() << "(data: data[ptr...], size: &" << var_name << "Size) else {" << std::endl
	<< "    return nil" << std::endl
	<< "}" << std::endl
	<< "ptr += " << var_name << "Size" << std::endl;
	// clang-format on
}

void SwiftMessageGenerator::generate_read_code(CodeOutput &output,
											   const MessageField &field) {
	const std::string message_type_name = field.type->identifier();
	const bool is_self_referencing =
		output.get_message_schema().message_name == message_type_name;
	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	// clang-format off
	output.stream()
	<< "let " << field_name_camel_case << "Size = data.readSize(ofField: " << field.field_number << ")" << std::endl;
	// clang-format on

	if (is_self_referencing) {
		if (!output.is_variable_in_scope(field_name_camel_case)) {
			// clang-format off
			output.stream()
			<< "let " << field_name_camel_case << ": " << field.type->identifier() << "?" << std::endl;
			// clang-format on
		}
		// clang-format off
		output.stream()
		<< "if " << field_name_camel_case << "Size < 0 {" << std::endl
		<< "    " << field_name_camel_case << " = nil" << std::endl
		<< "} else {" << std::endl
		<< "    " << field_name_camel_case << " = " << field.type->identifier() << "(data: data[ptr..<ptr + " << field_name_camel_case << "Size])" << std::endl
		<< "}" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< (output.is_variable_in_scope(field_name_camel_case) ? "" : "let ") << field_name_camel_case << " = " << field.type->identifier() << "(data: data[ptr..<ptr + " << field_name_camel_case << "Size])" << std::endl;
		// clang-format on
	}
}

void SwiftMessageGenerator::generate_write_code(CodeOutput &output,
												NanoPack::DataType *type,
												const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "guard let " << var_name << "Data = " << var_name << ".data() else {" << std::endl
	<< "    return nil" << std::endl
	<< "}" << std::endl
	<< "data.append(" << var_name << "Data)" << std::endl;
	// clang-format on
}

void SwiftMessageGenerator::generate_write_code(CodeOutput &output,
												const MessageField &field) {
	const std::string message_type_name = field.type->identifier();
	const bool is_self_referencing =
		output.get_message_schema().message_name == message_type_name;
	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	if (is_self_referencing) {
		// clang-format off
		output.stream()
		<< "if let " << field_name_camel_case << " = self." << field_name_camel_case << ", "
		<< "   let " << field_name_camel_case << "Data = " << field_name_camel_case << ".data() {" << std::endl
		<< "    data.append(" << field_name_camel_case << "Data)" << std::endl
		<< "    data.write(size: " << field_name_camel_case << "Data.count, ofField: " << field.field_number << ")" << std::endl
		<< "} else {" << std::endl
		<< "    data.write(size: -1, ofField: " << field.field_number << ")" << std::endl
		<< "}" << std::endl;
		// clang-format on
	} else {
		generate_write_code(output, field.type.get(), field.field_name);
		// clang-format off
		output.stream()
		<< "data.write(size: " << field.field_name << "Data.count, ofField: " << field.field_number << ")" << std::endl;
		// clang-format on
	}
}
