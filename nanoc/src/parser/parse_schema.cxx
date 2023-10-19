#include "parse_schema.hxx"
#include <filesystem>
#include <functional>
#include <optional>
#include <yaml.h>

std::optional<MessageSchema> parse_schema_file(const std::string &file_path) {
	yaml_parser_t parser;
	if (!yaml_parser_initialize(&parser)) {
		return std::nullopt;
	}

	FILE *fh = fopen(file_path.c_str(), "r");
	if (fh == nullptr) {
		return std::nullopt;
	}

	yaml_parser_set_input_file(&parser, fh);

	MessageSchema schema{
		.schema_path = std::filesystem::path(file_path),
		.type_id = -1,
	};
	yaml_token_t token;
	int i = 0;
	bool parse_failed = false;
	do {
		yaml_parser_scan(&parser, &token);

		if (token.type == YAML_STREAM_END_TOKEN) {
			break;
		}

		if (token.type == YAML_BLOCK_END_TOKEN) {
			i--;
			continue;
		}

		if ((i == 0 && token.type == YAML_STREAM_START_TOKEN) ||
			(i == 1 && token.type == YAML_BLOCK_MAPPING_START_TOKEN) ||
			(i == 2 && token.type == YAML_KEY_TOKEN) ||
			(i == 4 && token.type == YAML_VALUE_TOKEN) ||
			(i == 5 && token.type == YAML_BLOCK_MAPPING_START_TOKEN)) {
			i++;
			continue;
		}

		if ((i == 3 && token.type == YAML_SCALAR_TOKEN)) {
			schema.message_name =
				std::string(reinterpret_cast<char *>(token.data.scalar.value),
							token.data.scalar.length);
			i++;
			continue;
		}

		if (i == 6 && token.type == YAML_KEY_TOKEN) {
			// read next token for field name
			yaml_parser_scan(&parser, &token);

			std::string key_name(
				reinterpret_cast<char *>(token.data.scalar.value),
				token.data.scalar.length);

			yaml_parser_scan(&parser, &token);
			if (token.type != YAML_VALUE_TOKEN) {
				// invalid type id found
				parse_failed = true;
				break;
			}

			yaml_parser_scan(&parser, &token);
			if (token.type != YAML_SCALAR_TOKEN) {
				// invalid type id found
				parse_failed = true;
				break;
			}

			if (key_name == "type_id") {
				// type_id declaration encountered, treat current token as
				// value of type id
				const int type_id =
					atoi(reinterpret_cast<char *>(token.data.scalar.value));
				if (type_id == 0) {
					// invalid type id found
					parse_failed = true;
					break;
				}

				schema.type_id = type_id;
			} else {
				// field declaration encountered, treat current token as
				// type of the field
				std::string field_type(
					reinterpret_cast<char *>(token.data.scalar.value),
					token.data.scalar.length);

				// get field number
				// example: string:4, field number is 4
				const size_t pos = field_type.find_last_of(':');
				if (pos == std::string::npos) {
					// field number is not specified for this field, so it is
					// not valid
					parse_failed = true;
					break;
				}

				int field_number = atoi(field_type.substr(pos + 1).c_str());

				schema.fields.emplace_back(MessageField{
					.type_name = field_type.substr(0, pos),
					.field_name = key_name,
					.field_number = field_number,
				});
			}
			continue;
		}

		// invalid schema syntax
		parse_failed = true;
		break;
	} while (token.type != YAML_STREAM_END_TOKEN);

	yaml_parser_delete(&parser);

	if (parse_failed)
		return std::nullopt;

	// sort fields by field number in asc order
	std::sort(schema.fields.begin(), schema.fields.end(),
			  [](const MessageField &field_a, const MessageField &field_b) {
				  return field_a.field_number < field_b.field_number;
			  });

	return schema;
}
