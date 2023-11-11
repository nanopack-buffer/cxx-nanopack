#include "cxx_optional_generator.hxx"
#include "../../data_type/np_optional.hxx"

CxxOptionalGenerator::CxxOptionalGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
CxxOptionalGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	const auto optional_type = dynamic_cast<NanoPack::Optional *>(data_type);
	const std::shared_ptr<NanoPack::DataType> value_type =
		optional_type->get_value_type();
	const std::shared_ptr<DataTypeCodeGenerator> value_type_generator =
		generator_registry->find_generator_for_type(value_type.get());
	if (value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}
	return "std::optional<" +
		   value_type_generator->get_type_declaration(value_type.get()) + ">";
}

std::string
CxxOptionalGenerator::get_read_size_expression(NanoPack::DataType *data_type,
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
	return var_name + ".has_value() ? " +
		   value_type_generator->get_read_size_expression(
			   value_type.get(), var_name + ".value()") +
		   " : 1";
}

void CxxOptionalGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << get_type_declaration(field.type.get()) << " "
					<< field.field_name;
}

void CxxOptionalGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	output.stream() << field.field_name << "(" << field.field_name << ")";
}

void CxxOptionalGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << get_type_declaration(field.type.get())
					<< field.field_name << ";" << std::endl;
}

void CxxOptionalGenerator::generate_read_code(CodeOutput &output,
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
	<< get_type_declaration(type) << " " << var_name  << " = std::nullopt;" << std::endl
	<< "if (buf[ptr] != 0) {" << std::endl;
	// clang-format on

	// prevent value type generator from generating another variable declaration
	// since it is already declared above.
	output.add_variable_to_scope(var_name);
	value_type_generator->generate_read_code(output, value_type.get(),
											 var_name);

	output.stream() << "}" << std::endl << std::endl;
}

void CxxOptionalGenerator::generate_read_code(CodeOutput &output,
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

	// clang-format off
	output.stream()
	<< "if (reader.read_field_size(" << field.field_number << ") < 0) {" << std::endl
	<< "    this->" << field.field_name << " = std::nullopt;" << std::endl
	<< "} else {" << std::endl;
	// clang-format on

	MessageField value_field(field);
	value_field.type = value_type;
	value_type_generator->generate_read_code(output, value_field);

	output.stream() << "}" << std::endl;
}

void CxxOptionalGenerator::generate_write_code(CodeOutput &output,
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
	<< "if (" << var_name << ".has_value()) {" << std::endl
	<< "    writer.append_int8(1);" << std::endl
	<< "    auto " << var_name << "_value = " << var_name << ".value();" << std::endl;
			value_type_generator->generate_write_code(output, value_type.get(), var_name + "_value");
	output.stream()
	<< "} else {" << std::endl
	<< "    writer.append_int8(0);" << std::endl
	<< "}";
	// clang-format on
}

void CxxOptionalGenerator::generate_write_code(CodeOutput &output,
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

	// clang-format off
	output.stream()
	<< "if (" << field.field_name << ".has_value()) {" << std::endl
	<< "    auto " << field.field_name << " = this->" << field.field_name << ".value();" << std::endl;
	// clang-format on

	MessageField value_field(field);
	value_field.type = value_type;
	value_type_generator->generate_write_code(output, value_field);

	// clang-format off
	output.stream()
	<< "} else {" << std::endl
	<< "    writer.write_field_size(" << field.field_number << ", -1);" << std::endl
	<< "}" << std::endl
	<< std::endl;
	// clang-format on
}
