#include "cxx_map_generator.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_map.hxx"

CxxMapGenerator::CxxMapGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
CxxMapGenerator::get_type_declaration(NanoPack::DataType *data_type) {
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

	return "std::unordered_map<" +
		   key_type_generator->get_type_declaration(key_type) + ", " +
		   value_type_generator->get_type_declaration(value_type) + ">";
}

std::string
CxxMapGenerator::get_read_size_expression(NanoPack::DataType *data_type,
										  const std::string &var_name) {
	const auto map_type = dynamic_cast<NanoPack::Map *>(data_type);
	std::shared_ptr<NanoPack::DataType> key_type = map_type->get_key_type();
	std::shared_ptr<NanoPack::DataType> value_type = map_type->get_value_type();
	if (key_type->is_fixed_size() && value_type->is_fixed_size()) {
		return var_name + ".size() * (" + std::to_string(key_type->size()) +
			   " + " + std::to_string(value_type->size()) + ")";
	}
	return var_name + "_total_size";
}

void CxxMapGenerator::generate_field_declaration(CodeOutput &output,
												 const MessageField &field) {
	output.stream() << get_type_declaration(field.type.get()) << " "
					<< field.field_name << ";" << std::endl;
}

void CxxMapGenerator::generate_read_code(CodeOutput &output,
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

	// read, at the current read ptr, the number of elements that should be in
	// the map, and store the number to <var_name>_map_size
	int32_generator->generate_read_code(output, nullptr,
										var_name + "_map_size");

	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< var_name << " = " << get_type_declaration(type) << "()";
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		// instantiates a new map with <var_name>_vec_size as the number of elements in the map
		<< get_type_declaration(type) << var_name << ";" << std::endl;
		// clang-format on
	}

	output.stream() << var_name << ".reserve(" << var_name << "_map_size);"
					<< std::endl;

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
	<< "for (int " << loop_var << " = 0; " << loop_var << " < " << var_name << "_map_size; " << loop_var << "++) {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	key_type_generator->generate_read_code(output, key_type, loop_var + "_key");
	value_type_generator->generate_read_code(output, value_type,
											 loop_var + "_value");
	output.remove_variable_from_scope(loop_var);

	// clang-format off
	output.stream()
	<< var_name << ".insert({" << loop_var << "_key, " << loop_var << "_value});" << std::endl
	<< "}" << std::endl << std::endl;
	// clang-format on
}

void CxxMapGenerator::generate_read_code(CodeOutput &output,
										 const MessageField &field) {
	generate_read_code(output, field.type.get(), field.field_name + "_map");
	output.stream() << "this->" << field.field_name << " = " << field.field_name
					<< "_map;" << std::endl;
}

void CxxMapGenerator::generate_write_code(CodeOutput &output,
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
	<< "buf.append_int32(" << var_name << ".size());" << std::endl;
	// clang-format on

	if (!key_type->is_fixed_size() || !value_type->is_fixed_size()) {
		// declare a variable for storing the total byte size of all the
		// elements in the map, which is accumulated later in the loop

		output.stream() << "int32_t " << var_name
						<< "_total_size = sizeof(int32_t)";
		if (key_type->is_fixed_size()) {
			output.stream()
				<< " + " << var_name << ".size() * " << key_type->size();
		} else if (value_type->is_fixed_size()) {
			output.stream()
				<< " + " << var_name << ".size() * " << value_type->size();
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
	const std::string key_var = loop_var + "_key";
	const std::string value_var = loop_var + "_value";

	// clang-format off
	output.stream()
	<< "for (const auto& " << loop_var << " : " << var_name << ") {" << std::endl
	<< "auto " << key_var << " = " << loop_var << ".first;" << std::endl
	<< "auto " << value_var << " = " << loop_var << ".second;" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	output.add_variable_to_scope(key_var);
	output.add_variable_to_scope(value_var);
	key_type_generator->generate_write_code(output, key_type, key_var);
	value_type_generator->generate_write_code(output, value_type, value_var);
	output.remove_variable_from_scope(loop_var);
	output.remove_variable_from_scope(key_var);
	output.remove_variable_from_scope(value_var);

	if (!key_type->is_fixed_size()) {
		output.stream() << var_name << "_total_size += "
						<< key_type_generator->get_read_size_expression(
							   key_type, key_var)
						<< ";" << std::endl;
	}
	if (!value_type->is_fixed_size()) {
		output.stream() << var_name << "_total_size += "
						<< value_type_generator->get_read_size_expression(
							   value_type, value_var)
						<< ";" << std::endl;
	}

	output.stream() << "}" << std::endl << std::endl;
}

void CxxMapGenerator::generate_write_code(CodeOutput &output,
										  const MessageField &field) {
	const auto map_type = std::dynamic_pointer_cast<NanoPack::Map>(field.type);
	const std::shared_ptr<NanoPack::DataType> key_type =
		map_type->get_key_type();
	const std::shared_ptr<NanoPack::DataType> value_type =
		map_type->get_value_type();

	if (key_type->is_fixed_size() && value_type->is_fixed_size()) {
		output.stream() << "buf.write_field_size(" << field.field_number << ", "
						<< field.field_name << ".size() * ("
						<< std::to_string(key_type->size()) << " + "
						<< std::to_string(value_type->size()) << "))";
	}

	generate_write_code(output, field.type.get(), field.field_name);

	if (!key_type->is_fixed_size() || !value_type->is_fixed_size()) {
		output.stream() << "buf.write_field_size(" << field.field_number << ", "
						<< field.field_name << "_total_size);" << std::endl;
	}
}
