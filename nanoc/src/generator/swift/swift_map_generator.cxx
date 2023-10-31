#include "swift_map_generator.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_map.hxx"

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

void SwiftMapGenerator::generate_field_declaration(CodeOutput &output,
												   const MessageField &field) {
	output.stream() << "let " << field.field_name << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftMapGenerator::generate_read_code(CodeOutput &output,
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
										var_name + "ItemCount");

	// clang-format off
	output.stream()
	<< "var " << var_name << ": " << get_type_declaration(type) << " = [:]" << std::endl
	<< var_name << ".reserveCapacity(" << var_name << "ItemCount)" << std::endl;
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
