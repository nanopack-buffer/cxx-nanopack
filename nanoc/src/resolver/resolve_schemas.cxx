#include "resolve_schemas.hxx"
#include "oneapi/tbb/concurrent_unordered_map.h"

#include <list>

std::vector<MessageSchema> resolve_schemas(
	const oneapi::tbb::concurrent_unordered_map<std::string, ParseResult>
		&parse_results) {
	std::vector<MessageSchema> resolved_schemas;
	resolved_schemas.reserve(parse_results.size());

	// fill in all unresolved types
	for (auto &[message_name, parse_result] : parse_results) {
		if (parse_result.unresolved_parent_type != nullptr) {
			// find schema of parent type
			const auto entry = parse_results.find(
				parse_result.unresolved_parent_type->identifier());
			if (const bool parent_type_not_found = entry == parse_results.end();
				parent_type_not_found) {
				return {};
			}

			const std::shared_ptr<MessageSchema> parent_message_schema =
				entry->second.partial_schema;
			parent_message_schema->is_inherited = true;
			parse_result.partial_schema->parent_message = parent_message_schema;
		}

		{ // find all imported types
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
	}

	// adjust field numbers according to message inheritance
	// root message has the smallest field numbers
	for (MessageSchema &schema : resolved_schemas) {
		if (schema.parent_message == nullptr) {
			schema.all_fields.reserve(schema.declared_fields.size());
			schema.all_fields.insert(schema.all_fields.end(),
									 schema.declared_fields.begin(),
									 schema.declared_fields.end());
		} else {
			std::list<MessageSchema *> inheritance_chain;
			MessageSchema *current_schema = &schema;
			do {
				inheritance_chain.emplace_front(current_schema);
				current_schema = current_schema->parent_message.get();
			} while (current_schema != nullptr);

			int field_number = 0;
			int i = 0;
			const size_t last = inheritance_chain.size() - 1;
			for (MessageSchema *message : inheritance_chain) {
				for (MessageField &field : message->declared_fields) {
					field.field_number = field_number++;
				}
				if (i++ != last) {
					schema.inherited_fields.insert(
						schema.inherited_fields.end(),
						message->declared_fields.begin(),
						message->declared_fields.end());
				}
			}

			schema.all_fields.reserve(schema.declared_fields.size() +
									  schema.inherited_fields.size());
			schema.all_fields.insert(schema.all_fields.end(),
									 schema.inherited_fields.begin(),
									 schema.inherited_fields.end());
			schema.all_fields.insert(schema.all_fields.end(),
									 schema.declared_fields.begin(),
									 schema.declared_fields.end());
		}
	}

	return resolved_schemas;
}
