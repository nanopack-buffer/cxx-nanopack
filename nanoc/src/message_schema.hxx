#ifndef NANOPACK_NANOC_MESSAGE_SCHEMA_HXX
#define NANOPACK_NANOC_MESSAGE_SCHEMA_HXX

#include "data_type/data_type.hxx"
#include <filesystem>
#include <string>
#include <vector>

/**
 * Describes a field in a NanoPack message.
 */
struct MessageField {
	/**
	 * The data type of the field
	 */
	std::shared_ptr<NanoPack::DataType> type;

	std::string type_name;

	std::string field_name;

	int field_number;

	MessageField(std::shared_ptr<NanoPack::DataType> type, std::string typeName,
				 std::string fieldName, int fieldNumber);

	MessageField(const MessageField &other) = default;
};

/**
 * Describes a parsed NanoPack schema.
 */
struct MessageSchema {
	/**
	 * The absolute path to the NanoPack schema in the file system.
	 */
	std::filesystem::path schema_path;

	std::vector<std::shared_ptr<MessageSchema>> imported_messages;

	std::shared_ptr<MessageSchema> parent_message;

	/**
	 * The name of the top level message defined in the schema.
	 */
	std::string message_name;

	/**
	 * Type ID of the message.
	 */
	int type_id;

	std::vector<MessageField> inherited_fields;

	/**
	 * Fields declared in the message schema.
	 * This does not include inherited fields.
	 */
	std::vector<MessageField> declared_fields;

	std::vector<MessageField> all_fields;
};

#endif // NANOPACK_NANOC_MESSAGE_SCHEMA_HXX
