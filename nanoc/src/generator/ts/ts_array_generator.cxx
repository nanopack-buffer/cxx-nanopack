#include "ts_array_generator.hxx"

#include "../../data_type/np_array.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../string_util/case_conv.hxx"

TsArrayGenerator::TsArrayGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
TsArrayGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(data_type);
	const auto item_type = array_type->get_item_type().get();
	const auto item_type_generator =
		generator_registry->find_generator_for_type(item_type);
	if (item_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}
	return item_type_generator->get_type_declaration(item_type) + "[]";
}

std::string
TsArrayGenerator::get_read_size_expression(NanoPack::DataType *data_type,
										   const std::string &var_name) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(data_type);
	if (const std::shared_ptr item_type = array_type->get_item_type();
		item_type->is_fixed_size()) {
		return var_name + ".length * " + std::to_string(item_type->size());
	}
	return var_name + "ByteLength";
}

void TsArrayGenerator::generate_constructor_parameter(
	CodeOutput &output, const MessageField &field) {
	output.stream() << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void TsArrayGenerator::generate_constructor_field_initializer(
	CodeOutput &output, const MessageField &field) {
	const std::string field_name_snake_case = snake_to_camel(field.field_name);
	output.stream() << "this." << field_name_snake_case << " = "
					<< field_name_snake_case;
}

void TsArrayGenerator::generate_field_declaration(CodeOutput &output,
												  const MessageField &field) {
	// clang-format off
	output.stream()
	<< "public " << snake_to_camel(field.field_name) << ": " << get_type_declaration(field.type.get()) << ";" << std::endl;
	// clang-format on
}

void TsArrayGenerator::generate_read_code(CodeOutput &output,
										  NanoPack::DataType *type,
										  const std::string &var_name) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(type);
	const auto int32_generator = generator_registry->find_generator_for_type(
		NanoPack::Int32::IDENTIFIER);
	NanoPack::DataType *item_type = array_type->get_item_type().get();
	const auto item_type_generator =
		generator_registry->find_generator_for_type(item_type);
	if (int32_generator == nullptr || item_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	if (const std::string array_length_var_name = var_name + "Length";
		!output.is_variable_in_scope(array_length_var_name)) {
		// read, at the current read ptr, the number of elements that should be
		// in the result array, and store the number to <var_name>Length
		int32_generator->generate_read_code(output, nullptr,
											var_name + "Length");
	}

	// instantiates a new array with <var_name>Length as the number of
	// elements in the array
	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< var_name << " = new Array(" << var_name << "Length);" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< "const " << var_name << ": " << get_type_declaration(type) << " = new Array(" << var_name << "Length);" << std::endl;
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
	// keep reading data from the buffer until the array is filled.
	<< "for (let " << loop_var << " = 0; " << loop_var << " < " << var_name << "Length; " << loop_var << "++) {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	// generate code for reading buffer data into <loop_var>_item.
	item_type_generator->generate_read_code(
		output, array_type->get_item_type().get(), loop_var + "Item");
	output.remove_variable_from_scope(loop_var);

	// clang-format off
	output.stream()
	<< std::endl
	// store <loop_var>Item into the array.
	<< var_name << "[" << loop_var << "] = " << loop_var << "Item;" << std::endl
	<< "}";
	// clang-format on
}

void TsArrayGenerator::generate_read_code(CodeOutput &output,
										  const MessageField &field) {
	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	if (const auto array_type =
			std::dynamic_pointer_cast<NanoPack::Array>(field.type);
		array_type->get_item_type()->is_fixed_size()) {
		// for arrays with fixed size items, the number of elements in the array
		// can be calculated.

		// clang-format off
		output.stream()
		<< "const " << field_name_camel_case << "ByteLength = reader.readFieldSize(" << field.field_number << ");" << std::endl
		<< "const " << field_name_camel_case << "Length = " << field_name_camel_case << "ByteLength / " << array_type->get_item_type()->size() << ";" << std::endl;
		// clang-format on

		output.add_variable_to_scope(field_name_camel_case + "Length");
		output.add_variable_to_scope(field_name_camel_case + "ByteLength");
	}

	generate_read_code(output, field.type.get(), field_name_camel_case);
}

void TsArrayGenerator::generate_write_code(CodeOutput &output,
										   NanoPack::DataType *type,
										   const std::string &var_name) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(type);
	NanoPack::DataType *item_type = array_type->get_item_type().get();
	const auto item_type_generator =
		generator_registry->find_generator_for_type(item_type);
	if (item_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	// clang-format off
	output.stream()
	// append the number of elements in the array into the buffer at the write ptr.
	<< "writer.appendInt32(" << var_name << ".length);" << std::endl;
	// clang-format on

	if (!item_type->is_fixed_size()) {
		// declare a variable for storing the total byte size of all the
		// elements in the array, which is accumulated later in the loop

		// clang-format off
		output.stream()
		<< "let " << var_name << "ByteLength = 4;" << std::endl;
		// clang-format on
	}

	// finds a name for the loop variable that doesn't clash with existing
	// variables, because the loop can be nested inside another loop,
	// so simply using "i" as the loop var won't work.
	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	output.stream() << "for (const " << loop_var << " of " << var_name << ") {"
					<< std::endl;

	output.add_variable_to_scope(loop_var);
	// generate code for writing each element in the array into the buffer
	item_type_generator->generate_write_code(output, item_type, loop_var);

	if (!item_type->is_fixed_size()) {
		// store the byte size of the raw data of the item into the accumulator
		// variable declared earlier.
		output.stream() << var_name << "ByteLength += "
						<< item_type_generator->get_read_size_expression(
							   item_type, loop_var)
						<< ";" << std::endl;
	}

	output.stream() << "}" << std::endl;
	output.remove_variable_from_scope(loop_var);
}

void TsArrayGenerator::generate_write_code(CodeOutput &output,
										   const MessageField &field) {
	const auto array_type =
		std::dynamic_pointer_cast<NanoPack::Array>(field.type);
	std::shared_ptr<NanoPack::DataType> item_type = array_type->get_item_type();
	const auto item_type_generator =
		generator_registry->find_generator_for_type(item_type.get());
	if (item_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	const std::string field_name_camel_case = snake_to_camel(field.field_name);

	if (item_type->is_fixed_size()) {
		// the array has fixed size elements, so the total size of the array
		// data can be calculated directly:
		//
		//     sizeof(element type) * number of elements in the array +
		//
		// the number of elements in the array is not written to the buffer,
		// unlike elements with dynamic size, since it can be determined easily:
		//
		//     number of elements in the array = total byte size of array /
		//     sizeof(element type).
		//

		// clang-format off
		output.stream()
		<< "writer.writeFieldSize(" << field.field_number << ", " << item_type->size() << " * this." << field_name_camel_case << ".length);" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		// declare an accumulator variable to store the total size of the array
		<< "let " << field_name_camel_case << "ByteLength = 4;" << std::endl
		// append number of elements in the array before the actual array data
		<< "writer.appendInt32(this." << field_name_camel_case << ".length);" << std::endl;
		// clang-format on
	}

	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	// clang-format off
	output.stream()
	<< "for (const " << loop_var << " of this." << field_name_camel_case << ") {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	// generate code for writing each element in the array into the buffer
	item_type_generator->generate_write_code(output, item_type.get(), loop_var);

	if (!item_type->is_fixed_size()) {
		// store the byte size of the raw data of the item into the variable
		// declared earlier
		output.stream() << field_name_camel_case << "ByteLength += "
						<< item_type_generator->get_read_size_expression(
							   item_type.get(), loop_var)
						<< ";" << std::endl;
	}

	output.stream() << "}" << std::endl;
	output.remove_variable_from_scope(loop_var);

	if (!item_type->is_fixed_size()) {
		// write the accumulator variable declared earlier into the size header
		// for the field
		output.stream() << "writer.writeFieldSize(" << field.field_number
						<< ", " << field_name_camel_case << "ByteLength);"
						<< std::endl
						<< std::endl;
	}
}
