#ifndef NANOPACK_NANOC_MESSAGE_SCHEMA_HXX
#define NANOPACK_NANOC_MESSAGE_SCHEMA_HXX

#include "data_type/data_type.hxx"
#include <filesystem>
#include <string>
#include <vector>

struct MessageField {
	std::shared_ptr<DataType> type;
	std::string type_name;
	std::string field_name;
	int field_number;

	MessageField(std::shared_ptr<DataType> type, std::string typeName,
				 std::string fieldName, int fieldNumber);
};

struct MessageSchema {
	std::filesystem::path schema_path;
	std::string message_name;
	int type_id;
	std::vector<MessageField> fields;
};

#endif // NANOPACK_NANOC_MESSAGE_SCHEMA_HXX
