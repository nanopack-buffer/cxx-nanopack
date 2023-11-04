#include "swift_array_generator.hxx"
#include "../../data_type/np_array.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_int8.hxx"
#include "../../string_util/case_conv.hxx"

SwiftArrayGenerator::SwiftArrayGenerator(
	std::shared_ptr<DataTypeCodeGeneratorRegistry> generator_registry)
	: generator_registry(std::move(generator_registry)) {}

std::string
SwiftArrayGenerator::get_type_declaration(NanoPack::DataType *data_type) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(data_type);
	NanoPack::DataType *item_type = array_type->get_item_type().get();
	std::shared_ptr<DataTypeCodeGenerator> item_type_generator =
		generator_registry->find_generator_for_type(item_type);
	if (item_type_generator == nullptr) {
		throw "Array has unsupported type: " + item_type->identifier();
	}

	return "[" + item_type_generator->get_type_declaration(item_type) + "]";
}

std::string
SwiftArrayGenerator::get_read_size_expression(NanoPack::DataType *data_type,
											  const std::string &var_name) {
	const auto array_type = dynamic_cast<NanoPack::Array *>(data_type);
	std::shared_ptr item_type = array_type->get_item_type();
	if (item_type->is_fixed_size()) {
		return var_name + ".count * " + std::to_string(item_type->size());
	}
	return var_name + "TotalByteSize";
}

void SwiftArrayGenerator::generate_field_declaration(
	CodeOutput &output, const MessageField &field) {
	output.stream() << "let " << snake_to_camel(field.field_name) << ": "
					<< get_type_declaration(field.type.get()) << std::endl;
}

void SwiftArrayGenerator::generate_read_code(CodeOutput &output,
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

	// read, at the current read ptr, the number of elements that should be
	// in the result vector, and store the number to <var_name>ItemCount

	// clang-format off
	output.stream()
	<< "let " << var_name << "ItemCount = data.readUnalignedSize(at: ptr)" << std::endl
	<< "ptr += 4" << std::endl;
	// clang-format on

	if (item_type->is_fixed_size()) {
		// clang-format off
		output.stream()
		<< "let " << var_name << "Size = " << var_name << "ItemCount * " << item_type->size() << std::endl
		<< "let " << var_name << " = data[ptr..<ptr + " << var_name << "Size].withUnsafeBytes {" << std::endl
		<< get_type_declaration(type) << "($0.bindMemory(to: " << item_type_generator->get_type_declaration(item_type) << ".self)";
		// clang-format on

		const auto is_fixed_width_integer =
			item_type->identifier() == NanoPack::Int32::IDENTIFIER ||
			item_type->identifier() == NanoPack::Int8::IDENTIFIER;
		if (is_fixed_width_integer) {
			output.stream() << ".lazy.map { $0.littleEndian }";
		}

		output.stream() << ")" << std::endl << "}" << std::endl << std::endl;
	} else {
		// clang-format off
		output.stream()
		<< "var " << var_name << ": " << get_type_declaration(type) << " = []" << std::endl
		<< var_name << ".reserveCapacity(" << var_name << "ItemCount)" << std::endl
		<< "for _ in 0..<" << var_name << "ItemCount {" << std::endl;
		// clang-format on

		std::string item_var;
		int i = 0;
		do {
			item_var = "item" + std::to_string(i++);
		} while (output.is_variable_in_scope(item_var));

		output.add_variable_to_scope(item_var);
		item_type_generator->generate_read_code(output, item_type, item_var);
		output.remove_variable_from_scope(item_var);

		// clang-format off
		output.stream()
		<< var_name << ".append(" << item_var << ")" << std::endl
		<< "}" << std::endl;
		// clang-format on
	}
}

void SwiftArrayGenerator::generate_read_code(CodeOutput &output,
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

	const auto field_name_camel = snake_to_camel(field.field_name);

	if (item_type->is_fixed_size()) {
		// clang-format off
		output.stream()
		<< "let " << field_name_camel << "Size = data.readSize(ofField: " << field.field_number << ")" << std::endl
		<< "self." << field_name_camel << " = data[ptr..<ptr + " << field_name_camel << "Size].withUnsafeBytes {" << std::endl
		<< get_type_declaration(field.type.get()) << "($0.bindMemory(to: " << item_type_generator->get_type_declaration(item_type) << ".self)";
		// clang-format on

		const auto is_fixed_width_integer =
			item_type->identifier() == NanoPack::Int32::IDENTIFIER ||
			item_type->identifier() == NanoPack::Int8::IDENTIFIER;
		if (is_fixed_width_integer) {
			output.stream() << ".lazy.map { $0.littleEndian }";
		}

		// clang-format off
		output.stream()
		<< ")" << std::endl
		<< "}" << std::endl // withUnsafeBytes
		<< "ptr += " << field_name_camel << "Size" << std::endl
		<< std::endl;
		// clang-format on
	} else {
		generate_read_code(output, field.type.get(), field_name_camel);
		output.stream() << "self." << field_name_camel << " = "
						<< field_name_camel << std::endl
						<< std::endl;
	}
}

void SwiftArrayGenerator::generate_write_code(CodeOutput &output,
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
	<< "data.append(size: " << var_name << ".count);" << std::endl;
	// clang-format on

	if (!item_type->is_fixed_size()) {
		// declare a variable for storing the total byte size of all the
		// elements in the array, which is accumulated later in the loop

		// clang-format off
		output.stream()
		<< "let " << var_name << "TotalByteSize: Size = 4" << std::endl;
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

	output.stream() << "for " << loop_var << " in " << var_name << "{"
					<< std::endl;

	output.add_variable_to_scope(loop_var);
	// generate code for writing each element in the vector into the buffer
	item_type_generator->generate_write_code(output, item_type, loop_var);
	output.remove_variable_from_scope(loop_var);

	if (!item_type->is_fixed_size()) {
		// store the byte size of the raw data of the item into the accumulator
		// variable declared earlier.
		output.stream() << var_name << "TotalByteSize += "
						<< item_type_generator->get_read_size_expression(
							   item_type, loop_var)
						<< std::endl;
	}
}

void SwiftArrayGenerator::generate_write_code(CodeOutput &output,
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

	const auto field_name_camel = snake_to_camel(field.field_name);

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
		<< "data.write(size: " << item_type->size() << " * " << field_name_camel << ".count, ofField: " << field.field_number <<");" << std::endl;
		// clang-format on
	} else {
		// clang-format off
		output.stream()
		// declare an accumulator variable to store the total size of the array
		<< "var " << field_name_camel << "TotalByteSize = 4" << std::endl
		// append number of elements in the array before the actual array data
		<< "data.append(size: " << field_name_camel << ".count);" << std::endl;
		// clang-format on
	}

	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	output.stream() << "for " << loop_var << " in " << field_name_camel << "{"
					<< std::endl;

	output.add_variable_to_scope(loop_var);
	// generate code for writing each element in the vector into the buffer
	item_type_generator->generate_write_code(output, item_type, loop_var);
	output.remove_variable_from_scope(loop_var);

	if (!item_type->is_fixed_size()) {
		// store the byte size of the raw data of the item into the accumulator
		// variable declared earlier.
		output.stream() << field_name_camel << "TotalByteSize += "
						<< item_type_generator->get_read_size_expression(
							   item_type, loop_var)
						<< std::endl;
	}

	output.stream() << "}" << std::endl << std::endl;

	if (!item_type->is_fixed_size()) {
		// write the accumulator variable declared earlier into the size header
		// for the field
		output.stream() << "data.write(size: " << field_name_camel
						<< "TotalByteSize, ofField: " << field.field_number
						<< ");" << std::endl
						<< std::endl;
	}
}
