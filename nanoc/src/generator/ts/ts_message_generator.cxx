#include "ts_message_generator.hxx"

#include "../../string_util/case_conv.hxx"

std::string
TsMessageGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	return data_type->identifier();
}

std::string
TsMessageGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											 const std::string &var_name) {
	return var_name + "Data.byteLength";
}

void TsMessageGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	const std::string message_type_name = field.type->identifier();

	output.stream() << snake_to_camel(field.field_name) << ": "
					<< message_type_name;

	if (const bool is_self_referencing =
			output.get_message_schema().message_name == message_type_name;
		is_self_referencing) {
		output.stream() << " | null";
	}
}

void TsMessageGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "this." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void TsMessageGenerator::generate_field_declaration(CodeOutput &output,
													const MessageField &field) {
	const std::string message_type_name = field.type->identifier();
	const bool is_self_referencing =
		output.get_message_schema().message_name == message_type_name;

	output.stream() << "public " << snake_to_camel(field.field_name) << ": "
					<< message_type_name
					<< (is_self_referencing ? " | null;" : ";") << std::endl;
}

void TsMessageGenerator::generate_read_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "const maybe_" << var_name << " = " << type->identifier() << ".fromReader(reader.newReaderAt(ptr));" << std::endl
	<< "if (!maybe_" << var_name << ") {" << std::endl
	<< "    return null;" << std::endl
	<< "}" << std::endl
	<< (output.is_variable_in_scope(var_name) ? "" : "const ") << var_name << " = maybe_" << var_name << ".result;" << std::endl
	<< "ptr += maybe_" << var_name << ".bytesRead;" << std::endl;
	// clang-format on
}

void TsMessageGenerator::generate_read_code(CodeOutput &output,
											const MessageField &field) {
	const std::string message_type_name = field.type->identifier();
	const MessageSchema &schema = output.get_message_schema();
	const bool is_self_referencing = schema.message_name == message_type_name;
	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	if (is_self_referencing) {
		// clang-format off
		output.stream()
		<< "const " << field_name_camel_case << "Size = reader.readFieldSize(" << field.field_number << ")" << std::endl;
		// clang-format on

		if (!output.is_variable_in_scope(field_name_camel_case)) {
			output.stream()
				<< "let " << field_name_camel_case << "!: " << message_type_name
				<< " | null;" << std::endl;
		}

		// clang-format off
		output.stream()
		<< "if (" << field_name_camel_case << "Size < 0) {" << std::endl
		<< "    " << field_name_camel_case << " = null" << std::endl
		<< "} else {" << std::endl
		<< "    const maybe_" << field_name_camel_case << " = " << message_type_name << ".fromReader(reader.newReaderAt(ptr));" << std::endl
		<< "    if (!maybe_" << field_name_camel_case << ") {" << std::endl
		<< "        return null;" << std::endl
		<< "    }" << std::endl
		<< "    " << field_name_camel_case << " = maybe_" << field_name_camel_case << ".result;" << std::endl
		<< "    ptr += " << field_name_camel_case << "Size;" << std::endl
		<< "}" << std::endl;
		// clang-format on
	} else if (const auto imported_schema =
				   schema.find_imported_schema(field.type->identifier());
			   imported_schema != nullptr && imported_schema->is_inherited) {
		// if the message stored by this field is inherited
		// we use the make<MessageName> factory function to ensure]
		// we produce the correct concrete type of the message.

		// clang-format off
		output.stream()
		<< "const maybe_" << field_name_camel_case << " = make" << field.type->identifier() << "(reader.slice(ptr));" << std::endl
		<< "if (!maybe_" << field_name_camel_case << ") {" << std::endl
		<< "    return null;" << std::endl
		<< "}" << std::endl
		<< (output.is_variable_in_scope(field_name_camel_case) ? "" : "const ") << field_name_camel_case << " = maybe_" << field_name_camel_case << ".result;" << std::endl
		<< "ptr += maybe_" << field_name_camel_case << ".bytesRead;" << std::endl;
		// clang-format on
	} else {
		generate_read_code(output, field.type.get(), field_name_camel_case);
	}
}

void TsMessageGenerator::generate_write_code(CodeOutput &output,
											 NanoPack::DataType *type,
											 const std::string &var_name) {
	// clang-format off
	output.stream()
	<< "const " << var_name << "Data = " << var_name << ".bytes();" << std::endl
	<< "writer.appendBytes(" << var_name << "Data);" << std::endl;
	// clang-format on
}

void TsMessageGenerator::generate_write_code(CodeOutput &output,
											 const MessageField &field) {
	const std::string message_type_name = field.type->identifier();
	const bool is_self_referencing =
		output.get_message_schema().message_name == message_type_name;
	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	if (is_self_referencing) {
		// clang-format off
		output.stream()
		<< "if (this." << field_name_camel_case << ") {" << std::endl
		<< "    const " << field_name_camel_case << "Data = this." << field_name_camel_case << ".bytes();" << std::endl
		<< "    writer.appendBytes(" << field_name_camel_case << "Data);" << std::endl
		<< "    writer.writeFieldSize(" << field.field_number << ", " << field_name_camel_case << "Data.byteLength);" << std::endl
		<< "} else {"
		<< "    writer.writeFieldSize(" << field.field_number << ", -1);" << std::endl
		<< "}" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< "const " << field_name_camel_case << "Data = this." << field_name_camel_case << ".bytes();" << std::endl
		<< "writer.appendBytes(" << field_name_camel_case << "Data);" << std::endl
		<< "writer.writeFieldSize(" << field.field_number << ", " << field_name_camel_case << "Data.byteLength);" << std::endl;
		// clang-format on
	}
}
