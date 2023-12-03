#include "ts_generator.hxx"

#include "../../data_type/np_array.hxx"
#include "../../data_type/np_bool.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_int8.hxx"
#include "../../data_type/np_map.hxx"
#include "../../data_type/np_optional.hxx"
#include "../../data_type/np_string.hxx"
#include "../../string_util/case_conv.hxx"
#include "ts_array_generator.hxx"
#include "ts_bool_generator.hxx"
#include "ts_int32_generator.hxx"
#include "ts_int8_generator.hxx"
#include "ts_map_generator.hxx"
#include "ts_message_generator.hxx"
#include "ts_optional_generator.hxx"
#include "ts_string_generator.hxx"

#include <fstream>

const std::filesystem::path CODE_FILE_EXT(".np.ts");

TsGenerator::TsGenerator()
	: user_defined_message_type_generator(
		  std::make_shared<TsMessageGenerator>()),
	  data_type_generator_registry(
		  std::make_shared<DataTypeCodeGeneratorRegistry>()) {
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Bool::IDENTIFIER, std::make_shared<TsBoolGenerator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Int8::IDENTIFIER, std::make_shared<TsInt8Generator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Int32::IDENTIFIER, std::make_shared<TsInt32Generator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::String::IDENTIFIER, std::make_shared<TsStringGenerator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Array::IDENTIFIER,
		std::make_shared<TsArrayGenerator>(data_type_generator_registry));
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Map::IDENTIFIER,
		std::make_shared<TsMapGenerator>(data_type_generator_registry));
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Optional::IDENTIFIER,
		std::make_shared<TsOptionalGenerator>(data_type_generator_registry));
}

void TsGenerator::generate_for_schema(const MessageSchema &schema) {
	std::ofstream output_file_stream;
	CodeOutput code_output(output_file_stream, schema);
	std::filesystem::path output_path(schema.schema_path);
	output_path.replace_filename(snake_to_kebab(output_path.filename()))
		.replace_extension(CODE_FILE_EXT);

	output_file_stream.open(output_path);

	const bool has_parent_message = schema.parent_message != nullptr;

	// clang-format off
	code_output.stream()
	<< "// AUTOMATICALLY GENERATED BY NANOPACK. DO NOT MODIFY BY HAND." << std::endl
	<< std::endl
	<< "import { NanoBufReader, NanoBufWriter " << (has_parent_message ? "" : ", type NanoPackMessage") << "} from \"nanopack\"" << std::endl
	<< std::endl;
	// clang-format on

	if (has_parent_message) {
		std::filesystem::path parent_message_path(
			schema.parent_message->schema_path);
		parent_message_path.replace_extension(".np.js");
		std::filesystem::path import_path =
			relative(parent_message_path, schema.schema_path.parent_path());
		std::string import_path_string = import_path.string();

		if (import_path_string[0] != '.') {
			import_path_string.insert(0, "./");
		}

		code_output.stream()
			<< "import { " << schema.parent_message->message_name
			<< " } from \"" << import_path_string << "\";" << std::endl
			<< std::endl;
	}

	// clang-format off
	code_output.stream()
	<< "class " << schema.message_name << (has_parent_message ? " extends " + schema.parent_message->message_name : " implements NanoPackMessage") << "{" << std::endl
	<< "    public static TYPE_ID = " << schema.type_id << ";" << std::endl
	<< std::endl
	<< "    constructor(";
	// clang-format on

	for (const MessageField &field : schema.inherited_fields) {
		const std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator == nullptr)
			continue;

		generator->generate_constructor_parameter(code_output, field);
		code_output.stream() << ", ";
	}

	for (const MessageField &field : schema.declared_fields) {
		const std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator == nullptr)
			continue;

		code_output.stream() << "public ";
		generator->generate_constructor_parameter(code_output, field);
		code_output.stream() << ", ";
	}

	if (has_parent_message) {
		// clang-format off
		code_output.stream()
		<< ") {" << std::endl
		<< "    super(";
		// clang-format on
		for (const MessageField &field : schema.inherited_fields) {
			code_output.stream()
				<< snake_to_camel(field.field_name) << "," << std::endl;
		}
		code_output.stream() << ");" << std::endl << "}" << std::endl;
	} else {
		code_output.stream() << ") {}" << std::endl;
	}

	// clang-format off
	code_output.stream()
	<< std::endl
	<< "public static fromBytes(bytes: Uint8Array): { bytesRead: number, result: " << schema.message_name << " } | null {" << std::endl
	<< "    const reader = new NanoBufReader(bytes);" << std::endl
	<< "    if (reader.readTypeId() !== " << schema.message_name << ".TYPE_ID) {" << std::endl
	<< "        return null;" << std::endl
	<< "    }"
	<< std::endl
	<< "    let ptr = " << (schema.all_fields.size() + 1) * 4 << ";" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.all_fields) {
		const std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator == nullptr)
			continue;

		generator->generate_read_code(code_output, field);
		code_output.stream() << std::endl;
	}

	// clang-format off
	code_output.stream()
	<< "return { bytesRead: ptr, result: new " << schema.message_name << "(";
	// clang-format on

	for (const MessageField &field : schema.all_fields) {
		code_output.stream() << snake_to_camel(field.field_name) << ", ";
	}

	// clang-format off
	code_output.stream()
	<< ") };" << std::endl
	<< "}" << std::endl
	<< std::endl
	<< (has_parent_message ? "override " : "") << "public bytes(): Uint8Array {" << std::endl
	<< "    const writer = new NanoBufWriter(" << (schema.all_fields.size() + 1) * 4 << ");" << std::endl
	<< "    writer.writeTypeId(" << schema.message_name << ".TYPE_ID);" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.all_fields) {
		const std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator == nullptr)
			continue;

		generator->generate_write_code(code_output, field);
		code_output.stream() << std::endl;
	}

	// clang-format off
	code_output.stream()
	<< "    return writer.bytes;" << std::endl
	<< "}" << std::endl
	<< "}" << std::endl
	<< std::endl
	<< "export { " << schema.message_name << " };" << std::endl;
	// clang-format on

	output_file_stream.close();

	const std::string format_cmd =
		"npx prettier " + output_path.string() + " --write";
	system(format_cmd.c_str());
}

void TsGenerator::generate_message_factory(
	const std::vector<MessageSchema> &all_schemas,
	const std::filesystem::path &output_path) {
	std::ofstream file_stream;
	std::filesystem::path output_file_path(output_path);
	output_file_path.append("message-factory.ts");

	file_stream.open(output_file_path);

	// clang-format off
	file_stream
	<< "import type { NanoPackMessage } from \"nanopack\";" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageSchema &schema : all_schemas) {
		std::filesystem::path schema_path(schema.schema_path);
		schema_path.replace_extension(".np.js");

		std::filesystem::path import_path = relative(schema_path, output_path);
		std::string import_path_string = import_path.string();

		if (import_path_string[0] != '.') {
			import_path_string.insert(0, "./");
		}

		file_stream << "import { " << schema.message_name << " } from \""
					<< import_path_string << "\";" << std::endl;
	}

	// clang-format off
	file_stream
	<< std::endl
	<< "function makeNanoPackMessage(bytes: Uint8Array, typeId: number): { bytesRead: number, result: NanoPackMessage } | null {" << std::endl
	<< "    switch (typeId) {" << std::endl;
	// clang-format on

	for (const MessageSchema &schema : all_schemas) {
		file_stream << "case " << schema.type_id << ": return "
					<< schema.message_name << ".fromBytes(bytes);" << std::endl;
	}

	// clang-format off
	file_stream
	<< "    default: return null;" << std::endl
	<< "    }" << std::endl
	<< "}" << std::endl
	<< std::endl
	<< "export { makeNanoPackMessage };" << std::endl;
	// clang-format on

	const std::string format_cmd =
		"npx prettier " + output_file_path.string() + " --write";
	system(format_cmd.c_str());
}

std::shared_ptr<DataTypeCodeGenerator>
TsGenerator::find_generator_for_field(const MessageField &field) const {
	auto generator =
		data_type_generator_registry->find_generator_for_type(field.type.get());
	if (generator == nullptr)
		return user_defined_message_type_generator;
	return generator;
}
