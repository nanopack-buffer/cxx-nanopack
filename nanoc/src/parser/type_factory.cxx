#include "type_factory.hxx"
#include "../data_type/np_array.hxx"
#include "../data_type/np_int32.hxx"
#include "../data_type/np_int8.hxx"
#include "../data_type/np_map.hxx"
#include "../data_type/np_message.hxx"
#include "../data_type/np_optional.hxx"
#include "../data_type/np_string.hxx"

// TODO: perhaps set a limit on type nesting level?
std::unique_ptr<NanoPack::DataType>
NanoPack::create_type_from_literal(const std::string &literal) {
	const char last_char = literal.back();
	if (last_char == '?') {
		std::unique_ptr<NanoPack::DataType> value_type =
			create_type_from_literal(literal.substr(0, literal.length() - 1));
		if (value_type == nullptr) {
			return nullptr;
		}
		return std::make_unique<NanoPack::Optional>(std::move(value_type));
	}

	if (literal == NanoPack::Bool::IDENTIFIER) {
		return std::make_unique<NanoPack::Bool>();
	}
	if (literal == NanoPack::Int8::IDENTIFIER) {
		return std::make_unique<NanoPack::Int8>();
	}
	if (literal == NanoPack::Int32::IDENTIFIER) {
		return std::make_unique<NanoPack::Int32>();
	}
	if (literal == NanoPack::Double::IDENTIFIER) {
		return std::make_unique<NanoPack::Double>();
	}
	if (literal == NanoPack::String::IDENTIFIER) {
		return std::make_unique<NanoPack::String>();
	}
	if (literal == NanoPack::GenericMessage::IDENTIFIER) {
		return std::make_unique<NanoPack::GenericMessage>();
	}

	const auto array_identifier_pos =
		literal.rfind(NanoPack::Array::IDENTIFIER);
	if (array_identifier_pos == 0) {
		// array brackets cannot be the start of a type expression
		return nullptr;
	}

	const auto map_start_pos =
		literal.find(NanoPack::Map::MAP_IDENTIFIER_START);
	const auto map_end_pos = literal.rfind(NanoPack::Map::MAP_IDENTIFIER_END);

	const bool unmatched_map_brackets = (map_start_pos == std::string::npos) !=
										(map_end_pos == std::string::npos);
	const bool wrong_bracket_pos =
		map_start_pos != std::string::npos && map_start_pos != 0;
	if (unmatched_map_brackets || wrong_bracket_pos) {
		return nullptr;
	}

	if (array_identifier_pos == literal.length() - 2) {
		// array bracket is at the end of the type expression
		// which means "array of something"
		const auto array_type_identifier =
			literal.substr(0, array_identifier_pos);
		std::unique_ptr<NanoPack::DataType> array_type =
			create_type_from_literal(array_type_identifier);
		if (array_type == nullptr) {
			return nullptr;
		}
		return std::make_unique<NanoPack::Array>(std::move(array_type));
	}

	if (map_start_pos != std::string::npos &&
		map_end_pos != std::string::npos) {
		const auto map_type_expr =
			literal.substr(map_start_pos + 1, map_end_pos - map_start_pos - 1);
		const auto colon_pos = map_type_expr.find(':');

		const bool colon_is_start = colon_pos == 0;
		const bool colon_is_end = colon_pos == map_type_expr.length() - 1;
		const bool colon_not_found = colon_pos == std::string::npos;
		if (colon_is_start || colon_is_end || colon_not_found) {
			return nullptr;
		}

		const auto key_type_expr = map_type_expr.substr(0, colon_pos);
		const auto value_type_expr = map_type_expr.substr(colon_pos + 1);
		std::unique_ptr<NanoPack::DataType> key_type =
			create_type_from_literal(key_type_expr);
		std::unique_ptr<NanoPack::DataType> value_type =
			create_type_from_literal(value_type_expr);
		if (key_type == nullptr || value_type == nullptr) {
			return nullptr;
		}

		return std::make_unique<NanoPack::Map>(std::move(key_type),
											   std::move(value_type));
	}

	return std::make_unique<NanoPack::Message>(literal);
}
