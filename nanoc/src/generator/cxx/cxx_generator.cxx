#include "cxx_generator.hxx"
#include "../../data_type/np_bool.hxx"
#include "../../data_type/np_int8.hxx"
#include "../../data_type/np_string.hxx"
#include "../../string_util/case_conv.hxx"
#include "cxx_bool_generator.hxx"
#include "cxx_int8_generator.hxx"
#include "cxx_string_generator.hxx"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>

const std::filesystem::path HEADER_FILE_EXT(".np.hxx");
const std::filesystem::path CODE_FILE_EXT(".np.cxx");

CxxGenerator::CxxGenerator() : data_type_generators() {
	data_type_generators.insert({
		NanoPackBool::IDENTIFIER,
		new CxxBoolGenerator(),
	});
	data_type_generators.insert({
		NanoPackInt8::IDENTIFIER,
		new CxxInt8Generator(),
	});
	data_type_generators.insert({
		NanoPackString::IDENTIFIER,
		new CxxStringGenerator(),
	});
}

CxxGenerator::~CxxGenerator() {
	for (auto &data_type_generator : data_type_generators) {
		delete (data_type_generator.second);
	}
}

void CxxGenerator::generate_for_schema(const MessageSchema &schema) {
	const std::string header_file_name = generate_header_file(schema);
	generate_code_file(schema, header_file_name);
}

DataTypeCodeGenerator *
CxxGenerator::find_generator_for_field(const MessageField &field) {
	const auto pos = data_type_generators.find(field.type->identifier());
	if (pos == data_type_generators.end()) {
		return nullptr;
	}
	return pos->second;
}

std::string CxxGenerator::generate_header_file(const MessageSchema &schema) {
	std::ofstream output_file;
	std::filesystem::path output_path(schema.schema_path);
	output_path.replace_extension(HEADER_FILE_EXT);

	std::stringstream ss;
	// TODO: this doesn't work with namespaced message names
	ss << "__" << schema.message_name << "_NP_H__";

	std::string include_guard_name = ss.str();

	output_file.open(output_path);

	std::set<std::string> required_types;
	for (const MessageField &field : schema.fields) {
		required_types.insert(field.type->identifier());
	}

	// clang-format off
	output_file
	<< "// AUTOMATICALLY GENERATED BY NANOPACK. DO NOT MODIFY BY HAND." << std::endl
	<< std::endl
	<< "#ifndef " << include_guard_name << std::endl
	<< "#define " << include_guard_name << std::endl
	<< std::endl
	<< "#include <nanopack/nanobuf.hxx>" << std::endl;
	// clang-format on

	if (required_types.contains(NanoPackString::IDENTIFIER)) {
		output_file << "#include <string>" << std::endl;
	}
	// TODO: implement support for including custom message types

	// clang-format off
	output_file
	<< std::endl
	<< "struct " << schema.message_name << " {" << std::endl
	<< "private:" << std::endl
	<< "  static const int FIELD_COUNT = " << schema.fields.size() << ";" << std::endl
	<< std::endl
	<< "public:" << std::endl
	<< "  static const int32_t TYPE_ID = " << schema.type_id << ";" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.fields) {
		DataTypeCodeGenerator *generator = find_generator_for_field(field);
		if (generator != nullptr) {
			generator->generate_field_declaration(output_file, field);
			output_file << std::endl;
		}
	}

	// clang-format off
	output_file
	<< "  " << schema.message_name << "();" << std::endl
	<< std::endl
	<< "  explicit " << schema.message_name << "(std::vector<uint8_t> &data);" << std::endl
	<< std::endl
	<< "  NanoBuf data();" << std::endl
	<< "};" << std::endl
	<< std::endl
	<< "#endif" << std::endl;
	// clang-format on

	output_file.close();

	format_code_file(output_path);

	return output_path.filename();
}

void CxxGenerator::generate_code_file(const MessageSchema &schema,
									  const std::string &header_file_name) {
	std::ofstream output_file;
	std::filesystem::path output_path(schema.schema_path);
	output_path.replace_extension(CODE_FILE_EXT);

	output_file.open(output_path);

	// clang-format off
	output_file
	<< "#include \"" << header_file_name << "\"" << std::endl
	<< std::endl
	<< schema.message_name << "::" << schema.message_name << "() {}" << std::endl
	<< std::endl
	<< schema.message_name << "::" << schema.message_name << "(std::vector<uint8_t> &data) {" << std::endl
	<< "NanoBuf buf(data);" << std::endl
	<< "int ptr = " << 4 * (schema.fields.size() + 1) << ";" << std::endl
	<< std::endl
	<< "const int32_t type_id = buf.read_type_id();" << std::endl
	<< "if (type_id != " << schema.message_name << "::TYPE_ID) {" << std::endl
	<< "  throw \"incompatible type\";" << std::endl
	<< "}" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.fields) {
		const std::string field_name_snake_case =
			camel_to_snake(field.field_name);

		DataTypeCodeGenerator *generator = find_generator_for_field(field);
		if (generator == nullptr) {
			// TODO: should probably stop the code gen here since an unsupported
			// 		 type is found.
			continue;
		}

		generator->generate_read_code(output_file, field);
		output_file << std::endl;
	}

	output_file << "}" << std::endl << std::endl;

	// clang-format off
	output_file
	<< "NanoBuf " << schema.message_name << "::data() {" << std::endl
	<< "NanoBuf buf(sizeof(int32_t) * " << schema.fields.size() + 1 << ");" << std::endl
	<< std::endl
	<< "buf.write_type_id(" << schema.message_name << "::TYPE_ID);" << std::endl
	<< std::endl;
	// clang-format on

	for (const MessageField &field : schema.fields) {
		const std::string field_name_snake_case =
			camel_to_snake(field.field_name);

		DataTypeCodeGenerator *generator = find_generator_for_field(field);
		if (generator == nullptr) {
			// TODO: should probably stop the code gen here since an unsupported
			// 		 type is found.
			continue;
		}

		generator->generate_write_field_code(output_file, field);
		output_file << std::endl;
	}

	output_file << "return buf;" << std::endl << "}" << std::endl << std::endl;

	output_file.close();

	format_code_file(output_path);
}

void CxxGenerator::format_code_file(const std::filesystem::path &path) {
	const std::string format_cmd =
		"clang-format -i -style=LLVM " + path.string();
	system(format_cmd.c_str());
}
