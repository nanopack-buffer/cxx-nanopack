#include "ts_map_generator.hxx"

#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_map.hxx"
#include "../../string_util/case_conv.hxx"

TsMapGenerator::TsMapGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
TsMapGenerator::get_type_declaration(NanoPack::DataType *data_type) {
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

	return "Map<" + key_type_generator->get_type_declaration(key_type) + ", " +
		   value_type_generator->get_type_declaration(value_type) + ">";
}

std::string
TsMapGenerator::get_read_size_expression(NanoPack::DataType *data_type,
										 const std::string &var_name) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(data_type);
	std::shared_ptr<NanoPack::DataType> key_type = map_type->get_key_type();
	std::shared_ptr<NanoPack::DataType> value_type = map_type->get_value_type();
	if (key_type->is_fixed_size() && value_type->is_fixed_size()) {
		return var_name + ".length * (" + std::to_string(key_type->size()) +
			   " + " + std::to_string(value_type->size()) + ")";
	}
	return var_name + "ByteLength";
}

void TsMapGenerator::generate_constructor_parameter(CodeOutput &output,
													const MessageField &field) {
	// clang-format off
	output.stream()
	<< snake_to_camel(field.field_name) << ": " << get_type_declaration(field.type.get());
	// clang-format on
}

void TsMapGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);
	// clang-format off
	output.stream()
	<< "this." << field_name_camel_case << " = " << field_name_camel_case << ";" << std::endl;
	// clang-format on
}

void TsMapGenerator::generate_field_declaration(CodeOutput &output,
												const MessageField &field) {
	// clang-format off
	output.stream()
	<< "public " << snake_to_camel(field.field_name) << ": " << get_type_declaration(field.type.get()) << ";" << std::endl;
	// clang-format on
}

void TsMapGenerator::generate_read_code(CodeOutput &output,
										NanoPack::DataType *type,
										const std::string &var_name) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(type);
	const auto int32_generator = generator_registry->find_generator_for_type(
		NanoPack::Int32::IDENTIFIER);
	NanoPack::DataType *key_type = map_type->get_key_type().get();
	NanoPack::DataType *value_type = map_type->get_value_type().get();
	const auto key_type_generator =
		generator_registry->find_generator_for_type(key_type);
	const auto value_type_generator =
		generator_registry->find_generator_for_type(
			map_type->get_value_type().get());
	if (int32_generator == nullptr || key_type_generator == nullptr ||
		value_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	if (const std::string map_size_var_name = var_name + "MapSize";
		!output.is_variable_in_scope(map_size_var_name)) {
		int32_generator->generate_read_code(output, nullptr, map_size_var_name);
	}

	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< var_name << " = new" << get_type_declaration(type) << "()";
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		// instantiates a new map with <var_name>_vec_size as the number of elements in the map
		<< (output.is_variable_in_scope(var_name) ? "" : "const ") << var_name << " = new " << get_type_declaration(type) << "();" << std::endl;
		// clang-format on
	}

	// finds a name for the loop variable that doesn't clash with existing
	// variables, because the loop can be nested inside another loop,
	// so simply using "i" as the loop var won't work.
	std::string loop_var;
	int i = 1;
	do {
		loop_var = std::string(i++, 'i');
	} while (output.is_variable_in_scope(loop_var));

	// clang-format off
	output.stream()
	<< "for (let " << loop_var << " = 0; " << loop_var << " < " << var_name << "_map_size; " << loop_var << "++) {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	key_type_generator->generate_read_code(output, key_type, loop_var + "Key");
	value_type_generator->generate_read_code(output, value_type,
											 loop_var + "Value");

	// clang-format off
	output.stream()
	<< var_name << ".set(" << loop_var << "Key, " << loop_var << "Value);" << std::endl
	<< "}" << std::endl;
	// clang-format on

	output.remove_variable_from_scope(loop_var);
}

void TsMapGenerator::generate_read_code(CodeOutput &output,
										const MessageField &field) {
	generate_read_code(output, field.type.get(),
					   snake_to_camel(field.field_name));
}

void TsMapGenerator::generate_write_code(CodeOutput &output,
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
	<< "writer.appendInt32(" << var_name << ".size);" << std::endl;
	// clang-format on

	if (!key_type->is_fixed_size() || !value_type->is_fixed_size()) {
		// declare a variable for storing the total byte size of all the
		// elements in the map, which is accumulated later in the loop

		output.stream() << "let " << var_name << "ByteLength = 4";
		if (key_type->is_fixed_size()) {
			output.stream()
				<< " + " << var_name << ".size * " << key_type->size();
		} else if (value_type->is_fixed_size()) {
			output.stream()
				<< " + " << var_name << ".size * " << value_type->size();
		}
		output.stream() << ";" << std::endl;
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
	<< var_name << ".forEach((" << value_var << ", " << key_var << ") => {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(key_var);
	output.add_variable_to_scope(value_var);
	key_type_generator->generate_write_code(output, key_type, key_var);
	value_type_generator->generate_write_code(output, value_type, value_var);

	if (!key_type->is_fixed_size()) {
		output.stream() << var_name << "ByteLength += "
						<< key_type_generator->get_read_size_expression(
							   key_type, key_var)
						<< ";" << std::endl;
	}
	if (!value_type->is_fixed_size()) {
		output.stream() << var_name << "ByteLength += "
						<< value_type_generator->get_read_size_expression(
							   value_type, value_var)
						<< ";" << std::endl;
	}

	output.stream() << "});" << std::endl;
	output.remove_variable_from_scope(key_var);
	output.remove_variable_from_scope(value_var);
}

void TsMapGenerator::generate_write_code(CodeOutput &output,
										 const MessageField &field) {
	const auto map_type = std::dynamic_pointer_cast<NanoPack::Map>(field.type);
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

	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	// clang-format off
	output.stream()
	// append the number of elements in the vector into the buffer at the write ptr.
	<< "writer.appendInt32(this." << field_name_camel_case << ".size);" << std::endl;
	// clang-format on

	if (key_type->is_fixed_size() && value_type->is_fixed_size()) {
		output.stream() << "writer.writeFieldSize(" << field.field_number
						<< ", this." << field.field_name << ".size * ("
						<< std::to_string(key_type->size()) << " + "
						<< std::to_string(value_type->size()) << "))";
	} else {
		// declare a variable for storing the total byte size of all the
		// elements in the map, which is accumulated later in the loop

		output.stream() << "let " << field_name_camel_case << "ByteLength = 4";
		if (key_type->is_fixed_size()) {
			output.stream() << " + " << field_name_camel_case << ".size * "
							<< key_type->size();
		} else if (value_type->is_fixed_size()) {
			output.stream() << " + " << field_name_camel_case << ".size * "
							<< value_type->size();
		}
		output.stream() << ";" << std::endl;
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
	<< "this." << field_name_camel_case << ".forEach((" << value_var << ", " << key_var << ") => {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(key_var);
	output.add_variable_to_scope(value_var);
	key_type_generator->generate_write_code(output, key_type, key_var);
	value_type_generator->generate_write_code(output, value_type, value_var);

	if (!key_type->is_fixed_size()) {
		output.stream() << field_name_camel_case << "ByteLength += "
						<< key_type_generator->get_read_size_expression(
							   key_type, key_var)
						<< ';' << std::endl;
	}
	if (!value_type->is_fixed_size()) {
		output.stream() << field_name_camel_case << "ByteLength += "
						<< value_type_generator->get_read_size_expression(
							   value_type, value_var)
						<< ';' << std::endl;
	}

	output.stream() << "});" << std::endl;
	output.remove_variable_from_scope(key_var);
	output.remove_variable_from_scope(value_var);
}
