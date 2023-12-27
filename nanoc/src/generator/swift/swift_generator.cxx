#include "swift_generator.hxx"
#include "../../data_type/np_array.hxx"
#include "../../data_type/np_bool.hxx"
#include "../../data_type/np_double.hxx"
#include "../../data_type/np_int32.hxx"
#include "../../data_type/np_int64.hxx"
#include "../../data_type/np_int8.hxx"
#include "../../data_type/np_map.hxx"
#include "../../data_type/np_optional.hxx"
#include "../../data_type/np_string.hxx"
#include "../../string_util/case_conv.hxx"
#include "swift_array_generator.hxx"
#include "swift_bool_generator.hxx"
#include "swift_double_generator.hxx"
#include "swift_int32_generator.hxx"
#include "swift_int64_generator.hxx"
#include "swift_int8_generator.hxx"
#include "swift_map_generator.hxx"
#include "swift_message_generator.hxx"
#include "swift_optional_generator.hxx"
#include "swift_string_generator.hxx"

#include <filesystem>
#include <fstream>

const std::filesystem::path CODE_FILE_EXT(".np.swift");

void format_code(const std::filesystem::path &path) {
	const std::string format_cmd = "swift-format --in-place " + path.string();
	system(format_cmd.c_str());
}

SwiftGenerator::SwiftGenerator()
	: user_defined_message_type_generator(
		  std::make_shared<SwiftMessageGenerator>()),
	  data_type_generator_registry(
		  std::make_shared<DataTypeCodeGeneratorRegistry>()) {
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Bool::IDENTIFIER, std::make_shared<SwiftBoolGenerator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Int8::IDENTIFIER, std::make_shared<SwiftInt8Generator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Int32::IDENTIFIER, std::make_shared<SwiftInt32Generator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Int64::IDENTIFIER, std::make_shared<SwiftInt64Generator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Double::IDENTIFIER, std::make_shared<SwiftDoubleGenerator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::String::IDENTIFIER, std::make_shared<SwiftStringGenerator>());
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Array::IDENTIFIER,
		std::make_shared<SwiftArrayGenerator>(data_type_generator_registry));
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Map::IDENTIFIER,
		std::make_shared<SwiftMapGenerator>(data_type_generator_registry));
	data_type_generator_registry->add_generator_for_type(
		NanoPack::Optional::IDENTIFIER,
		std::make_shared<SwiftOptionalGenerator>(data_type_generator_registry));
	data_type_generator_registry->set_message_generator(
		user_defined_message_type_generator);
}

void SwiftGenerator::generate_for_schema(const MessageSchema &schema) {
	std::ofstream output_file_stream;
	CodeOutput code_output(output_file_stream, schema);
	std::filesystem::path output_path(schema.schema_path);
	output_path.replace_filename(snake_to_pascal(output_path.filename()))
		.replace_extension(CODE_FILE_EXT);

	output_file_stream.open(output_path);

	const bool has_parent_message = schema.parent_message != nullptr;

	// clang-format off
	code_output.stream()
	<< "// AUTOMATICALLY GENERATED BY NANOPACK. DO NOT MODIFY BY HAND." << std::endl
	<< std::endl
	<< "import Foundation" << std::endl
	<< "import NanoPack" << std::endl
	<< std::endl
	<< "let " << schema.message_name << "_typeID: TypeID = " << schema.type_id << std::endl
	<< std::endl
	<< "class " << schema.message_name << " : " << (has_parent_message ? schema.parent_message->message_name : "NanoPackMessage") << " {" << std::endl;
	// clang-format on

	for (const MessageField &field : schema.declared_fields) {
		std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator != nullptr) {
			generator->generate_field_declaration(code_output, field);
		}
	}

	{
		code_output.stream() << std::endl << "    init(";
		size_t i = 0;
		const size_t last = schema.all_fields.size() - 1;
		for (const MessageField &field : schema.all_fields) {
			std::shared_ptr<DataTypeCodeGenerator> generator =
				find_generator_for_field(field);
			if (generator == nullptr)
				continue;

			generator->generate_constructor_parameter(code_output, field);
			if (i++ < last) {
				code_output.stream() << ", ";
			}
		}
		code_output.stream() << ") {" << std::endl;
	}

	for (const MessageField &field : schema.declared_fields) {
		std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator != nullptr) {
			generator->generate_constructor_field_initializer(code_output,
															  field);
			code_output.stream() << std::endl;
		}
	}

	if (has_parent_message) {
		code_output.stream() << "super.init(";
		size_t i = 0;
		const size_t last = schema.inherited_fields.size() - 1;
		for (const MessageField &inherited_field : schema.inherited_fields) {
			const std::string field_name_camel_case =
				snake_to_camel(inherited_field.field_name);
			code_output.stream()
				<< field_name_camel_case << ": " << field_name_camel_case;
			if (i++ < last) {
				code_output.stream() << ", ";
			}
		}
		code_output.stream() << ")" << std::endl;
	}

	code_output.stream() << "}" << std::endl << std::endl;

	if (schema.is_inherited) {
		// clang-format off
		code_output.stream()
		<< "static func from(data: Data) -> " << schema.message_name << "? {" << std::endl
		<< "    switch data.readTypeID() {" << std::endl
		<< "    case " << schema.message_name << "_typeID: return " << schema.message_name << "(data: data)" << std::endl;
		// clang-format on

		for (const std::shared_ptr<MessageSchema> &child_message :
			 schema.child_messages) {
			// clang-format off
			code_output.stream()
			<< "case " << child_message->message_name << "_typeID: return " << child_message->message_name << "(data: data)" << std::endl;
			// clang-format on
		}

		// clang-format off
		code_output.stream()
		<< "    default: return nil" << std::endl
		<< "    }" << std::endl
		<< "}" << std::endl
		<< std::endl;
		// clang-format on
	}

	// clang-format off
	code_output.stream()
	<< "    required init?(data: Data) {" << std::endl
	<< "        var ptr = " << (schema.all_fields.size() + 1) * 4 << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.all_fields) {
		std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator != nullptr) {
			generator->generate_read_code(code_output, field);
			code_output.stream() << std::endl;
		}
	}

	for (const MessageField &declared_field : schema.declared_fields) {
		const std::string field_name_camel_case =
			snake_to_camel(declared_field.field_name);
		code_output.stream() << "self." << field_name_camel_case << " = "
							 << field_name_camel_case << std::endl;
	}

	if (has_parent_message) {
		code_output.stream() << "super.init(";
		size_t i = 0;
		const size_t last = schema.inherited_fields.size() - 1;
		for (const MessageField &inherited_field : schema.inherited_fields) {
			const std::string field_name_camel_case =
				snake_to_camel(inherited_field.field_name);
			code_output.stream()
				<< field_name_camel_case << ": " << field_name_camel_case;
			if (i++ < last) {
				code_output.stream() << ", ";
			}
		}
		code_output.stream() << ")" << std::endl;
	}

	// clang-format off
	code_output.stream()
	<< "    }" << std::endl // end init
	<< std::endl
	<< "    " << (has_parent_message ? "override " : "") << "func data() -> Data? {" << std::endl
	<< "        var data = Data()" << std::endl
	<< "        data.reserveCapacity(" << (schema.all_fields.size() + 1) * 4 << ")" << std::endl
	<< std::endl
	<< "        withUnsafeBytes(of: Int32(" << schema.message_name << "_typeID)) {" << std::endl
	<< "            data.append(contentsOf: $0)" << std::endl
	<< "        }" << std::endl
	<< std::endl
	<< "        data.append([0], count: " << schema.all_fields.size() * 4 << ")" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.all_fields) {
		std::shared_ptr<DataTypeCodeGenerator> generator =
			find_generator_for_field(field);
		if (generator != nullptr) {
			generator->generate_write_code(code_output, field);
			code_output.stream() << std::endl;
		}
	}

	// clang-format off
	code_output.stream()
	<< "        return data" << std::endl
	<< "    }" << std::endl // end func data()
	<< "}" << std::endl; // end extension
	// clang-format on

	output_file_stream.close();

	format_code(output_path);
}

void SwiftGenerator::generate_message_factory(
	const std::vector<MessageSchema> &all_schemas,
	const std::filesystem::path &output_path) {
	std::ofstream file_stream;
	std::filesystem::path output_file_path(output_path);
	output_file_path.append("NanoPackMessageFactory.swift");

	file_stream.open(output_file_path);

	// clang-format off
	file_stream
	<< "import Foundation" << std::endl
	<< "import NanoPack" << std::endl
	<< std::endl
	<< "func makeNanoPackMessage(from data: Data, typeID: TypeID) -> NanoPackMessage? {" << std::endl
	<< "    switch typeID {" << std::endl;
	// clang-format on

	for (const MessageSchema &schema : all_schemas) {
		file_stream << "case " << schema.type_id << ": return "
					<< schema.message_name << "(data: data)" << std::endl;
	}

	// clang-format off
	file_stream
	<< "    default: return nil" << std::endl
	<< "    }" << std::endl
	<< "}" << std::endl;
	// clang-format on

	file_stream.close();

	format_code(output_file_path);
}

std::shared_ptr<DataTypeCodeGenerator>
SwiftGenerator::find_generator_for_field(const MessageField &field) {
	const auto generator =
		data_type_generator_registry->find_generator_for_type(field.type.get());
	if (generator == nullptr) {
		return user_defined_message_type_generator;
	}
	return generator;
}
