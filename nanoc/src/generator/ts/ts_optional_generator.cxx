#include "ts_optional_generator.hxx"

#include "../../data_type/np_optional.hxx"
#include "../../string_util/case_conv.hxx"

namespace NanoPack {
class Optional;
}
TsOptionalGenerator::TsOptionalGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
TsOptionalGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	const auto optional_type = dynamic_cast<NanoPack::Optional *>(data_type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}
	return value_type_generator->get_type_declaration(value_type.get()) +
		   " | null";
}

std::string
TsOptionalGenerator::get_read_size_expression(NanoPack::DataType *data_type,
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

	return var_name + " ? " +
		   value_type_generator->get_read_size_expression(value_type.get(),
														  var_name) +
		   " : 1";
}

void TsOptionalGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get());
}

void TsOptionalGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "this." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void TsOptionalGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "public " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get());
}

void TsOptionalGenerator::generate_read_code(CodeOutput &output,
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
	<< "let " << var_name << "!: " << get_type_declaration(type) << " = null;" << std::endl
	<< "if (reader.readBoolean(ptr++)) {" << std::endl;
	// clang-format on

	// prevent value type generator from generating another variable declaration
	// since it is already declared above.
	output.add_variable_to_scope(var_name);
	value_type_generator->generate_read_code(output, value_type.get(),
											 var_name);

	output.stream() << "}" << std::endl;
}

void TsOptionalGenerator::generate_read_code(CodeOutput &output,
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

	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	// clang-format off
	output.stream()
	<< "let " << field_name_camel_case << ": " << get_type_declaration(optional_type.get()) << ";"
	<< "if (reader.readFieldSize(" << field.field_number << ") < 0) {" << std::endl
	<< "    " << field_name_camel_case << " = null;" << std::endl
	<< "} else {" << std::endl;
	// clang-format on

	// prevent value type generator from generating another variable declaration
	// since it is already declared above.
	output.add_variable_to_scope(field_name_camel_case);

	MessageField value_field(field);
	value_field.type = value_type;
	value_type_generator->generate_read_code(output, value_field);

	output.stream() << "}" << std::endl;
}

void TsOptionalGenerator::generate_write_code(CodeOutput &output,
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
	<< "if (" << var_name << ") {" << std::endl
	<< "    writer.appendBoolean(true);" << std::endl;
	// clang-format on
	value_type_generator->generate_write_code(output, value_type.get(),
											  var_name);

	// clang-format off
	output.stream()
	<< "} else {" << std::endl
	<< "    writer.appendBoolean(false);" << std::endl
	<< "}" << std::endl;
	// clang-format on
}

void TsOptionalGenerator::generate_write_code(CodeOutput &output,
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

	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	// clang-format off
	output.stream()
	<< "if (this." << field_name_camel_case << ") {" << std::endl;
	// clang-format on

	MessageField value_field(field);
	value_field.type = value_type;
	value_type_generator->generate_write_code(output, field);

	// clang-format off
	output.stream()
	<< "} else {"
	<< "    writer.writeFieldSize(" << field.field_number << ", -1);" << std::endl
	<< "}" << std::endl;
	// clang-format on
}
