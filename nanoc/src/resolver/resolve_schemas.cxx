#include "resolve_schemas.hxx"
#include "oneapi/tbb/concurrent_unordered_map.h"

std::vector<MessageSchema> resolve_schemas(
	const oneapi::tbb::concurrent_unordered_map<std::string, ParseResult>
		&parse_results) {
	std::vector<MessageSchema> resolved_schemas;
	resolved_schemas.reserve(parse_results.size());
	for (auto &[message_name, parse_result] : parse_results) {
		for (const std::shared_ptr<NanoPack::DataType> &unresolved_type :
			 parse_result.unresolved_types) {
			const auto entry =
				parse_results.find(unresolved_type->identifier());
			if (entry == parse_results.end()) {
				return {};
			}
			parse_result.partial_schema->imported_messages.emplace_back(
				entry->second.partial_schema);
		}
		resolved_schemas.emplace_back(*parse_result.partial_schema);
	}
	return resolved_schemas;
}
