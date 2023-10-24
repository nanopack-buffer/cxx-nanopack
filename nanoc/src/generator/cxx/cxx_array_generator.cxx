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

std::string CxxArrayGenerator::get_read_size_expression(const std::string &var_name) {

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
	const auto item_type_generator =
		generator_registry->find_generator_for_type(
			array_type->get_item_type().get());
	if (int32_generator == nullptr || item_type_generator == nullptr) {
		// TODO: better error handling
		throw "something";
	}

	int32_generator->generate_read_code(output, nullptr,
										var_name + "_vec_size");
	// clang-format off
	output.stream()
	<< get_type_declaration(type) << var_name << "(" << var_name << "_vec_size);" << std::endl;
	// clang-format on

	std::string loop_var;
	int i = 1;
	do {
		loop_var = std::string(i++, 'i');
	} while (output.is_variable_in_scope(loop_var));

	// clang-format off
	output.stream()
	<< "for (int " << loop_var << " = 0; " << loop_var << " < " << var_name << "_vec_size; " << loop_var << "++) {" << std::endl;
	// clang-format on

	output.add_variable_to_scope(loop_var);
	item_type_generator->generate_read_code(
		output, array_type->get_item_type().get(), loop_var + "_item");
	output.remove_variable_from_scope(loop_var);

	// clang-format off
	output.stream()
	<< std::endl
	<< var_name << "[" << loop_var << "] = " << loop_var << "_item;" << std::endl
	<< "}";
	// clang-format on
}

void CxxArrayGenerator::generate_read_code(CodeOutput &output,
										   const MessageField &field) {
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

	output.stream() << "buf.append_int32(" << var_name << ".size());"
					<< std::endl;

	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	output.stream() << "for (auto " << loop_var << " : " << var_name << ") {"
					<< std::endl;

	output.add_variable_to_scope(loop_var);
	item_type_generator->generate_write_code(output, item_type, loop_var);
	output.remove_variable_from_scope(loop_var);

	output.stream() << "}" << std::endl << std::endl;
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
		// clang-format off
		output.stream()
		<< "buf.write_field_size(" << field.field_number << ", " << item_type->size() << " * " << field.field_name << ".size() + 1);" << std::endl;
		// clang-format on
	} else {
		output.stream() << "int32_t total_size_written = sizeof(int32_t)";
	}

	output.stream() << "buf.append_int32(" << field.field_name << ".size());"
					<< std::endl;

	std::string loop_var;
	int i = 0;
	do {
		loop_var = "item" + std::to_string(i++);
	} while (output.is_variable_in_scope(loop_var));

	output.stream() << "for (auto " << loop_var << " : " << field.field_name
					<< ") {" << std::endl;
}
