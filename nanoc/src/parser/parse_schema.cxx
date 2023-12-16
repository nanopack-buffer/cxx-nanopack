#include "parse_schema.hxx"
#include "../data_type/np_message.hxx"
#include "symbols.hxx"
#include "type_factory.hxx"

#include <filesystem>
#include <optional>
#include <yaml.h>

struct TypeExpression {
	std::unique_ptr<NanoPack::DataType> data_type;
	int field_number;
};

struct MessageNameDeclaration {
	std::unique_ptr<NanoPack::DataType> parent_message;
	std::string message_name;
};

std::optional<TypeExpression>
parse_type_expression(const std::string &expression) {
	// get field number
	// example: string:4, field number is 4
	const size_t pos =
		expression.find_last_of(SchemaSymbol::FIELD_NUMBER_SEPERATOR);

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
	std::unique_ptr<NanoPack::DataType> data_type =
		NanoPack::create_type_from_literal(type_literal);
	if (data_type == nullptr) {
		return std::nullopt;
	}

	return TypeExpression{
		.data_type = std::move(data_type),
		.field_number = static_cast<int>(field_number),
	};
}

MessageNameDeclaration
parse_message_name_declaration(const std::string &message_name_declaration) {
	const size_t pos =
		message_name_declaration.find(SchemaSymbol::PARENT_TYPE_SEPERATOR);

	if (const bool has_parent_type = pos != std::string::npos;
		has_parent_type) {
		const std::string message_name =
			message_name_declaration.substr(0, pos);
		const std::string parent_message_name =
			message_name_declaration.substr(pos + 2);
		return {.parent_message =
					std::make_unique<NanoPack::Message>(parent_message_name),
				.message_name = message_name};
	}

	return {.parent_message = nullptr,
			.message_name = message_name_declaration};
}

std::optional<ParseResult> parse_schema_file(const std::filesystem::path &file_path) {
	yaml_parser_t parser;
	if (!yaml_parser_initialize(&parser)) {
		return std::nullopt;
	}

	FILE *fh = fopen(file_path.c_str(), "r");
	if (fh == nullptr) {
		return std::nullopt;
	}

	yaml_parser_set_input_file(&parser, fh);

	ParseResult parse_result;
	auto schema = std::make_shared<MessageSchema>();
	schema->schema_path = std::filesystem::path(file_path);
	schema->type_id = -1;
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

		if (i == 3 && token.type == YAML_SCALAR_TOKEN) {
			auto [parent_message, message_name] =
				parse_message_name_declaration(std::string(
					reinterpret_cast<char *>(token.data.scalar.value),
					token.data.scalar.length));

			parse_result.unresolved_parent_type = std::move(parent_message);
			schema->message_name = message_name;

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

			if (key_name == SchemaSymbol::KEYWORD_TYPE_ID) {
				// type_id declaration encountered, treat current token as
				// value of type idnt3
				const int type_id =
					atoi(reinterpret_cast<char *>(token.data.scalar.value));
				if (type_id <= 0) {
					// invalid type id found
					parse_failed = true;
					break;
				}

				schema->type_id = type_id;
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

				std::shared_ptr declared_type =
					std::move(type_expression->data_type);

				if (declared_type->is_user_defined()) {
					parse_result.unresolved_types.emplace_back(declared_type);
				}

				const std::string data_type_identifier =
					declared_type->identifier();

				schema->declared_fields.emplace_back(declared_type, data_type_identifier,
											key_name,
											type_expression->field_number);
			}
			continue;
		}

		// invalid schema syntax
		parse_failed = true;
		break;
	} while (token.type != YAML_STREAM_END_TOKEN);

	yaml_parser_delete(&parser);
	fclose(fh);

	if (parse_failed)
		return std::nullopt;

	// sort fields by field number in asc order
	std::sort(schema->declared_fields.begin(), schema->declared_fields.end(),
			  [](const MessageField &field_a, const MessageField &field_b) {
				  return field_a.field_number < field_b.field_number;
			  });

	parse_result.partial_schema = std::move(schema);

	return parse_result;
}
