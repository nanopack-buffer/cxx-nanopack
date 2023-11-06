#include "cxx_array_generator.hxx"
#include "../../data_type/np_array.hxx"
#include "../../data_type/np_int32.hxx"
#include <sstream>

CxxArrayGenerator::CxxArrayGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
CxxArrayGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	if (data_type == nullptr) {
		throw "Cannot deduce type declaration - not enough type information.";
	}

	auto array_type = dynamic_cast<NanoPack::Array *>(data_type);
	std::shared_ptr<NanoPack::DataType> item_type = array_type->get_item_type();
	auto item_type_generator =
		generator_registry->find_generator_for_type(item_type.get());
	if (item_type_generator == nullptr) {
		throw "Array has unsupported type: " + item_type->identifier();
	}

	return "std::vector<" +
		   item_type_generator->get_type_declaration(item_type.get()) + ">";
}

std::string
CxxArrayGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											const std::string &var_name) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(data_type);
	std::shared_ptr item_type = array_type->get_item_type();
	if (item_type->is_fixed_size()) {
		return var_name + ".size() * " + std::to_string(item_type->size());
	}
	return var_name + "_total_size";
}

void CxxArrayGenerator::generate_field_declaration(CodeOutput &output,
												   const MessageField &field) {
	output.stream() << get_type_declaration(field.type.get())
					<< field.field_name << ";" << std::endl;
}

void CxxArrayGenerator::generate_read_code(CodeOutput &output,
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

	if (!item_type->is_fixed_size()) {
		// read, at the current read ptr, the number of elements that should be
		// in the result vector, and store the number to <var_name>_vec_size
		int32_generator->generate_read_code(output, nullptr,
											var_name + "_vec_size");
	}

	// instantiates a new vector with <var_name>_vec_size as the number of
	// elements in the vector
	if (output.is_variable_in_scope(var_name)) {
		// clang-format off
		output.stream()
		<< var_name << " = " << get_type_declaration(type) << "(" << var_name << "_vec_size);" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		<< get_type_declaration(type) << var_name << "(" << var_name << "_vec_size);" << std::endl;
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
	// keep reading data from the buffer until the vector is filled.
	<< "for (int " << loop_var << " = 0; " << loop_var << " < " << var_name << "_vec_size; " << loop_var << "++) {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	// generate code for reading buffer data into <loop_var>_item.
	item_type_generator->generate_read_code(
		output, array_type->get_item_type().get(), loop_var + "_item");
	output.remove_variable_from_scope(loop_var);

	// clang-format off
	output.stream()
	<< std::endl
	// store <loop_var>_item into the vector.
	<< var_name << "[" << loop_var << "] = " << loop_var << "_item;" << std::endl
	<< "}";
	// clang-format on
}

void CxxArrayGenerator::generate_read_code(CodeOutput &output,
										   const MessageField &field) {
	const auto array_type =
		std::dynamic_pointer_cast<NanoPack::Array>(field.type);

	if (array_type->get_item_type()->is_fixed_size()) {
		// for arrays with fixed size items, the number of elements in the array
		// can be calculated.

		// clang-format off
		output.stream()
		<< "const int32_t " << field.field_name << "_size = buf.read_field_size(" << field.field_number << ");" << std::endl
		<< "const int32_t " << field.field_name << "_vec_vec_size = " << field.field_name << "_size / " << array_type->get_item_type()->size() << ";" << std::endl;
		// clang-format on
	}

	generate_read_code(output, field.type.get(), field.field_name + "_vec");
	output.stream() << "this->" << field.field_name << " = " << field.field_name
					<< "_vec;" << std::endl;
}

void CxxArrayGenerator::generate_write_code(CodeOutput &output,
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
	// append the number of elements in the vector into the buffer at the write ptr.
	<< "buf.append_int32(" << var_name << ".size());" << std::endl;
	// clang-format on

	if (!item_type->is_fixed_size()) {
		// declare a variable for storing the total byte size of all the
		// elements in the array, which is accumulated later in the loop

		// clang-format off
		output.stream()
		<< "int32_t " << var_name << "_total_size = sizeof(int32_t);" << std::endl;
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

	output.stream() << "for (auto " << loop_var << " : " << var_name << ") {"
					<< std::endl;

	output.add_variable_to_scope(loop_var);
	// generate code for writing each element in the vector into the buffer
	item_type_generator->generate_write_code(output, item_type, loop_var);
	output.remove_variable_from_scope(loop_var);

	if (!item_type->is_fixed_size()) {
		// store the byte size of the raw data of the item into the accumulator
		// variable declared earlier.
		output.stream() << var_name << "_total_size += "
						<< item_type_generator->get_read_size_expression(
							   item_type, loop_var)
						<< ";" << std::endl;
	}
}

void CxxArrayGenerator::generate_write_code(CodeOutput &output,
											const MessageField &field) {
	const auto array_type =
		std::dynamic_pointer_cast<NanoPack::Array>(field.type);
	NanoPack::DataType *item_type = array_type->get_item_type().get();
	const auto item_type_generator =
		generator_registry->find_generator_for_type(item_type);
	if (item_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	if (item_type->is_fixed_size()) {
		// the array has fixed size elements, so the total size of the array
		// data can be calculated directly:
		//
		//     sizeof(element type) * number of elements in the vector +
		//
		// the number of elements in the array is not written to the buffer,
		// unlike elements with dynamic size, since it can be determined easily:
		//
		//     number of elements in the vector = total byte size of vector /
		//     sizeof(element type).
		//

		// clang-format off
		output.stream()
		<< "buf.write_field_size(" << field.field_number << ", " << item_type->size() << " * " << field.field_name << ".size());" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		// declare an accumulator variable to store the total size of the array
		<< "int32_t " << field.field_name << "_total_size = sizeof(int32_t);" << std::endl
		// append number of elements in the array before the actual array data
		<< "buf.append_int32(" << field.field_name << ".size());" << std::endl;
		// clang-format on
	}

	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	// clang-format off
	output.stream()
	<< "for (const auto &" << loop_var << " : " << field.field_name << ") {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	// generate code for writing each element in the vector into the buffer
	item_type_generator->generate_write_code(output, item_type, loop_var);
	output.remove_variable_from_scope(loop_var);

	if (!item_type->is_fixed_size()) {
		// store the byte size of the raw data of the item into the variable
		// declared earlier
		output.stream() << field.field_name << "_total_size += "
						<< item_type_generator->get_read_size_expression(
							   item_type, loop_var)
						<< " + sizeof(int32_t);" << std::endl;
	}

	output.stream() << "}" << std::endl << std::endl;

	if (!item_type->is_fixed_size()) {
		// write the accumulator variable declared earlier into the size header
		// for the field
		output.stream() << "buf.write_field_size(" << field.field_number << ", "
						<< field.field_name << "_total_size);" << std::endl
						<< std::endl;
	}
}
