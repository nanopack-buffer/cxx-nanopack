#include "type_factory.hxx"
#include "np_array.hxx"
#include "np_int32.hxx"
#include "np_int8.hxx"
#include "np_map.hxx"
#include "np_string.hxx"

// TODO: perhaps set a limit on type nesting level?
std::shared_ptr<NanoPack::DataType>
NanoPack::create_type_from_identifier(const std::string &identifier) {
	if (identifier == NanoPack::Bool::IDENTIFIER) {
		return std::make_shared<NanoPack::Bool>();
	}
	if (identifier == NanoPack::Int8::IDENTIFIER) {
		return std::make_shared<NanoPack::Int8>();
	}
	if (identifier == NanoPack::Int32::IDENTIFIER) {
		return std::make_shared<NanoPack::Int32>();
	}
	if (identifier == NanoPack::Double::IDENTIFIER) {
		return std::make_shared<NanoPack::Double>();
	}
	if (identifier == NanoPack::String::IDENTIFIER) {
		return std::make_shared<NanoPack::String>();
	}

	const auto array_identifier_pos =
		identifier.rfind(NanoPack::Array::IDENTIFIER);
	if (array_identifier_pos == 0) {
		// array brackets cannot be the start of a type expression
		return nullptr;
	}

	const auto map_start_pos =
		identifier.find(NanoPack::Map::MAP_IDENTIFIER_START);
	const auto map_end_pos =
		identifier.rfind(NanoPack::Map::MAP_IDENTIFIER_END);

	const bool unmatched_map_brackets = (map_start_pos == std::string::npos) !=
										(map_end_pos == std::string::npos);
	const bool wrong_bracket_pos = map_start_pos != std::string::npos && map_start_pos != 0;
	if (unmatched_map_brackets || wrong_bracket_pos) {
		return nullptr;
	}

	if (array_identifier_pos == identifier.length() - 2) {
		// array bracket is at the end of the type expression
		// which means "array of something"
		const auto array_type_identifier =
			identifier.substr(0, array_identifier_pos);
		const auto array_type =
			create_type_from_identifier(array_type_identifier);
		if (array_type == nullptr) {
			return nullptr;
		}
		return std::make_shared<NanoPack::Array>(array_type);
	}

	if (map_start_pos != std::string::npos &&
		map_end_pos != std::string::npos) {
		const auto map_type_expr =
			identifier.substr(map_start_pos + 1, map_end_pos - map_start_pos - 1);
		const auto colon_pos = map_type_expr.find(':');

		const bool colon_is_start = colon_pos == 0;
		const bool colon_is_end = colon_pos == map_type_expr.length() - 1;
		const bool colon_not_found = colon_pos == std::string::npos;
		if (colon_is_start || colon_is_end || colon_not_found) {
			return nullptr;
		}

		const auto key_type_expr = map_type_expr.substr(0, colon_pos);
		const auto value_type_expr = map_type_expr.substr(colon_pos + 1);
		const auto key_type = create_type_from_identifier(key_type_expr);
		const auto value_type = create_type_from_identifier(value_type_expr);
		if (key_type == nullptr || value_type == nullptr) {
			return nullptr;
		}

		return std::make_shared<NanoPack::Map>(key_type, value_type);
	}

	// TODO: support message type as type literal
	return nullptr;
}
