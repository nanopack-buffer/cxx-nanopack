#include "parse_schema.hxx"
#include "../data_type/type_factory.hxx"
#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <yaml.h>

struct TypeExpression {
	std::shared_ptr<NanoPack::DataType> data_type;
	int field_number;
};

std::optional<TypeExpression>
parse_type_expression(const std::string &expression) {
	// get field number
	// example: string:4, field number is 4
	const size_t pos = expression.find_last_of(':');

	const bool no_type_name = pos == 0;
	const bool no_colon_found = pos == std::string::npos;
	const bool no_field_number_declared = pos == expression.length() - 1;
	if (no_type_name || no_colon_found || no_field_number_declared) {
		return std::nullopt;
	}

	errno = 0;
	const long field_number =
		std::strtol(expression.substr(pos + 1).c_str(), nullptr, 10);
	const bool field_number_out_of_range =
		field_number > std::numeric_limits<int>::max() ||
		field_number < std::numeric_limits<int>::min();
	if (errno != 0 || field_number_out_of_range) {
		// the specified field number in the expression is invalid
		// note: 0 is not considered a valid type id!
		return std::nullopt;
	}

	const std::string type_literal = expression.substr(0, pos);
	std::shared_ptr<NanoPack::DataType> data_type =
		NanoPack::create_type_from_identifier(type_literal);
	if (data_type == nullptr) {
		return std::nullopt;
	}

	return TypeExpression{
		.data_type = data_type,
		.field_number = static_cast<int>(field_number),
	};
}

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
				if (type_id <= 0) {
					// invalid type id found
					parse_failed = true;
					break;
				}

				schema.type_id = type_id;
			} else {
				// field declaration encountered, treat current token as
				// type of the field
				std::string type_expression_str(
					reinterpret_cast<char *>(token.data.scalar.value),
					token.data.scalar.length);

				std::optional<TypeExpression> type_expression =
					parse_type_expression(type_expression_str);
				if (!type_expression.has_value()) {
					parse_failed = true;
					break;
				}

				schema.fields.emplace_back(
					type_expression->data_type,
					type_expression->data_type->identifier(), key_name,
					type_expression->field_number);
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
