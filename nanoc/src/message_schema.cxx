#include <utility>

#include "message_schema.hxx"

MessageField::MessageField(std::shared_ptr<NanoPack::DataType> type,
						   std::string type_name, std::string field_name,
						   int field_number)
	: type(std::move(type)), type_name(std::move(type_name)),
	  field_name(std::move(field_name)), field_number(field_number) {}

std::shared_ptr<MessageSchema>
MessageSchema::find_imported_schema(const std::string &message_name) const {
	const auto it = std::find_if(
		imported_messages.begin(), imported_messages.end(),
		[message_name](const std::shared_ptr<MessageSchema> &schema) {
			return schema->message_name == message_name;
		});
	if (it == std::end(imported_messages)) {
		return nullptr;
	}
	return *it;
}
