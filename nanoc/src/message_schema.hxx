#ifndef NANOPACK_NANOC_MESSAGE_SCHEMA_HXX
#define NANOPACK_NANOC_MESSAGE_SCHEMA_HXX

#include <filesystem>
#include <string>
#include <vector>

struct MessageField {
	std::string type_name;
	std::string field_name;
	int field_number;
};

struct MessageSchema {
	std::filesystem::path schema_path;
	std::string message_name;
	int type_id;
	std::vector<MessageField> fields;
};

#endif // NANOPACK_NANOC_MESSAGE_SCHEMA_HXX
