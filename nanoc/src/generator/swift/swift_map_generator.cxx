#include "swift_map_generator.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_map.hxx"
#include "../../string_util/case_conv.hxx"

SwiftMapGenerator::SwiftMapGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
SwiftMapGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(data_type);
	NanoPack::DataType *key_type = map_type->get_key_type().get();
	NanoPack::DataType *value_type = map_type->get_value_type().get();
	const auto key_type_generator =
		generator_registry->find_generator_for_type(key_type);
	const auto value_type_generator =
		generator_registry->find_generator_for_type(value_type);
	if (key_type_generator == nullptr || value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	return "[" + key_type_generator->get_type_declaration(key_type) + ":" +
		   value_type_generator->get_type_declaration(value_type) + "]";
}

std::string
SwiftMapGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											const std::string &var_name) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(data_type);
	std::shared_ptr<NanoPack::DataType> key_type = map_type->get_key_type();
	std::shared_ptr<NanoPack::DataType> value_type = map_type->get_value_type();
	if (key_type->is_fixed_size() && value_type->is_fixed_size()) {
		return var_name + ".count * (" + std::to_string(key_type->size()) +
			   " + " + std::to_string(value_type->size()) + ")";
	}
	return var_name + "TotalByteSize";
}

void SwiftMapGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get());
}

void SwiftMapGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	output.stream() << "self." << field_name_camel_case << " = "
					<< field_name_camel_case;
}

void SwiftMapGenerator::generate_field_declaration(CodeOutput &output,
												   const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftMapGenerator::generate_read_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(type);
	NanoPack::DataType *key_type = map_type->get_key_type().get();
	NanoPack::DataType *value_type = map_type->get_value_type().get();
	const auto key_type_generator =
		generator_registry->find_generator_for_type(key_type);
	const auto value_type_generator =
		generator_registry->find_generator_for_type(
			map_type->get_value_type().get());
	if (key_type_generator == nullptr || value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	// clang-format off
	output.stream()
	// read, at the current read ptr, the number of elements that should be in
	// the map, and store the number to <var_name>_map_size
	<< "let " << var_name << "ItemCount = data.readUnalignedSize(at: ptr)" << std::endl;
	// clang-format on

	if (!output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< "var " << var_name << ": " << get_type_declaration(type) << " = [:]" << std::endl;
		// clang-format on
	}

	// clang-format off
	output.stream() << var_name << ".reserveCapacity(" << var_name << "ItemCount)" << std::endl;
	// clang-format on

	// finds a name for the loop variable that doesn't clash with existing
	// variables, because the loop can be nested inside another loop,
	// so simply using "i" as the loop var won't work.
	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	// clang-format off
	output.stream()
	<< "for _ in 0..<" << var_name << "ItemCount {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	key_type_generator->generate_read_code(output, key_type, loop_var + "Key");
	value_type_generator->generate_read_code(output, value_type,
											 loop_var + "Value");
	output.remove_variable_from_scope(loop_var);

	// clang-format off
	output.stream()
	<< var_name << "[" << loop_var << "Key] = " << loop_var << "Value" << std::endl
	<< "}" << std::endl // for
	<< std::endl;
	// clang-format on
}

void SwiftMapGenerator::generate_read_code(CodeOutput &output,
										   const MessageField &field) {
	const auto map_type = std::dynamic_pointer_cast<NanoPack::Map>(field.type);
	NanoPack::DataType *key_type = map_type->get_key_type().get();
	NanoPack::DataType *value_type = map_type->get_value_type().get();
	const auto key_type_generator =
		generator_registry->find_generator_for_type(key_type);
	const auto value_type_generator =
		generator_registry->find_generator_for_type(
			map_type->get_value_type().get());
	if (key_type_generator == nullptr || value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	const auto field_name_camel = snake_to_camel(field.field_name);

	// clang-format off
	output.stream()
	// read, at the current read ptr, the number of elements that should be in
	// the map, and store the number to <var_name>_map_size
	<< "let " << field_name_camel << "ItemCount = data.readUnalignedSize(at: ptr)" << std::endl
	<< "let " << field_name_camel << " = [:]" << std::endl
	<< field_name_camel << ".reserveCapacity(" << field_name_camel << "ItemCount)" << std::endl;
	// clang-format on

	// finds a name for the loop variable that doesn't clash with existing
	// variables, because the loop can be nested inside another loop,
	// so simply using "i" as the loop var won't work.
	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	// clang-format off
	output.stream()
	<< "for _ in 0..<" << field_name_camel << "ItemCount {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	key_type_generator->generate_read_code(output, key_type, loop_var + "Key");
	value_type_generator->generate_read_code(output, value_type,
											 loop_var + "Value");
	output.remove_variable_from_scope(loop_var);

	// clang-format off
	output.stream()
	<< field_name_camel << "[" << loop_var << "Key] = " << loop_var << "Value" << std::endl
	<< "}" << std::endl // for
	<< std::endl;
	// clang-format on
}

void SwiftMapGenerator::generate_write_code(CodeOutput &output,
											NanoPack::DataType *type,
											const std::string &var_name) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(type);
	NanoPack::DataType *key_type = map_type->get_key_type().get();
	NanoPack::DataType *value_type = map_type->get_value_type().get();
	const auto key_type_generator =
		generator_registry->find_generator_for_type(key_type);
	const auto value_type_generator =
		generator_registry->find_generator_for_type(value_type);
	if (key_type_generator == nullptr || value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	// clang-format off
	output.stream()
	// append the number of elements in the vector into the buffer at the write ptr.
	<< "data.append(size: " << var_name << ".count);" << std::endl;
	// clang-format on

	if (!key_type->is_fixed_size() || !value_type->is_fixed_size()) {
		// declare a variable for storing the total byte size of all the
		// elements in the map, which is accumulated later in the loop

		output.stream() << "var " << var_name
						<< "TotalByteSize = MemoryLayout<Size>.size";
		if (key_type->is_fixed_size()) {
			output.stream()
				<< " + " << var_name << ".count * " << key_type->size();
		} else if (value_type->is_fixed_size()) {
			output.stream()
				<< " + " << var_name << ".count * " << value_type->size();
		}
		output.stream() << std::endl;
	}

	// finds a name for the loop variable that doesn't clash with existing
	// variables, because the loop can be nested inside another loop,
	// so simply using "i" as the loop var won't work.
	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));
	const std::string key_var = loop_var + "Key";
	const std::string value_var = loop_var + "Value";

	// clang-format off
	output.stream()
	<< "for (" << key_var << ", " << value_var << ") in " << var_name << " {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(key_var);
	output.add_variable_to_scope(value_var);
	key_type_generator->generate_write_code(output, key_type, key_var);
	value_type_generator->generate_write_code(output, value_type, value_var);
	output.remove_variable_from_scope(key_var);
	output.remove_variable_from_scope(value_var);

	if (!key_type->is_fixed_size()) {
		output.stream() << var_name << "TotalByteSize += "
						<< key_type_generator->get_read_size_expression(
							   key_type, key_var)
						<< std::endl;
	}
	if (!value_type->is_fixed_size()) {
		output.stream() << var_name << "TotalByteSize += "
						<< value_type_generator->get_read_size_expression(
							   value_type, value_var)
						<< std::endl;
	}

	output.stream() << "}" << std::endl << std::endl;
}

void SwiftMapGenerator::generate_write_code(CodeOutput &output,
											const MessageField &field) {
	const auto map_type = std::dynamic_pointer_cast<NanoPack::Map>(field.type);
	const std::shared_ptr<NanoPack::DataType> key_type =
		map_type->get_key_type();
	const std::shared_ptr<NanoPack::DataType> value_type =
		map_type->get_value_type();

	const auto field_name_camel = snake_to_camel(field.field_name);

	if (key_type->is_fixed_size() && value_type->is_fixed_size()) {
		// clang-format off
		output.stream()
		<< "data.write(size: " << field_name_camel << ".count * (" << std::to_string(key_type->size()) << " + " << std::to_string(value_type->size()) << "), ofField: " << field.field_number << ")";
		// clang-format on
	}

	generate_write_code(output, field.type.get(), field_name_camel);

	if (!key_type->is_fixed_size() || !value_type->is_fixed_size()) {
		// clang-format off
		output.stream()
		<< "data.write(size: " << field_name_camel << "TotalByteSize, ofField: " << field.field_number << ")";
		// clang-format on
	}
}
