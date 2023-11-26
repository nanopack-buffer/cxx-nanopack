#include "swift_optional_generator.hxx"
#include "../../data_type/np_optional.hxx"
#include "../../string_util/case_conv.hxx"

SwiftOptionalGenerator::SwiftOptionalGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}
std::string

SwiftOptionalGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	const auto optional_type = dynamic_cast<NanoPack::Optional *>(data_type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}
	return value_type_generator->get_type_declaration(value_type.get()) + "?";
}

std::string
SwiftOptionalGenerator::get_read_size_expression(NanoPack::DataType *data_type,
												 const std::string &var_name) {
	const auto optional_type = dynamic_cast<NanoPack::Optional *>(data_type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	return var_name + ".map { " +
		   value_type_generator->get_read_size_expression(value_type.get(),
														  "$0") +
		   " } ?? 1";
}

void SwiftOptionalGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get());
}

void SwiftOptionalGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "self." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void SwiftOptionalGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftOptionalGenerator::generate_read_code(CodeOutput &output,
												NanoPack::DataType *type,
												const std::string &var_name) {
	const auto optional_type = dynamic_cast<NanoPack::Optional *>(type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	// clang-format off
	output.stream()
	<< "var " << var_name << ": " << get_type_declaration(type) << " = nil" << std::endl
	<< "if data[ptr] != 0 {" << std::endl;
	// clang-format on

	// prevent value type generator from generating another variable declaration
	// since it is already declared above.
	output.add_variable_to_scope(var_name);
	value_type_generator->generate_read_code(output, value_type.get(),
											 var_name);

	output.stream() << "}" << std::endl << std::endl;
}

void SwiftOptionalGenerator::generate_read_code(CodeOutput &output,
												const MessageField &field) {
	const auto optional_type =
		std::dynamic_pointer_cast<NanoPack::Optional>(field.type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	const std::string field_name_camel = snake_to_camel(field.field_name);

	if (!output.is_variable_in_scope(field_name_camel)) {
		// clang-format off
		output.stream()
		<< "let " << field_name_camel << ": " << get_type_declaration(field.type.get()) << std::endl;
		// clang-format on
		output.add_variable_to_scope(field_name_camel);
	}

	// clang-format off
	output.stream()
	<< "if data.readSize(ofField: " << field.field_number << ") < 0 {" << std::endl
	<< "    " << field_name_camel << " = nil" << std::endl
	<< "} else {" << std::endl;
	// clang-format on

	MessageField value_field(field);
	value_field.type = value_type;
	value_type_generator->generate_read_code(output, value_field);

	output.stream() << "}" << std::endl << std::endl;
}

void SwiftOptionalGenerator::generate_write_code(CodeOutput &output,
												 NanoPack::DataType *type,
												 const std::string &var_name) {
	const auto optional_type = dynamic_cast<NanoPack::Optional *>(type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	// clang-format off
	output.stream()
	<< "if let " << var_name << "{" << std::endl
	<< "    data.append(byte: 1)" << std::endl;
	// clang-format on
	value_type_generator->generate_write_code(output, value_type.get(),
											  var_name);

	// clang-format off
	output.stream()
	<< "} else {" << std::endl
	<< "    data.append(byte: 0)" << std::endl
	<< "}" << std::endl;
	// clang-format on
}

void SwiftOptionalGenerator::generate_write_code(CodeOutput &output,
												 const MessageField &field) {
	const auto optional_type =
		std::dynamic_pointer_cast<NanoPack::Optional>(field.type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	const std::string field_name_camel = snake_to_camel(field.field_name);

	// clang-format off
	output.stream()
	<< "if let " << field_name_camel << " = self." << field_name_camel << "{" << std::endl;
	// clang-format on

	MessageField value_field(field);
	value_field.type = value_type;
	value_type_generator->generate_write_code(output, field);

	// clang-format off
	output.stream()
	<< "} else {" << std::endl
	<< "    data.write(size: -1, ofField: " << field.field_number << ")" << std::endl
	<< "}" << std::endl
	<< std::endl;
	// clang-format on
}
