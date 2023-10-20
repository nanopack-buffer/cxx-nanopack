#include <utility>

#include "message_schema.hxx"

MessageField::MessageField(std::shared_ptr<DataType> type,
						   std::string type_name, std::string field_name,
						   int field_number)
	: type(std::move(type)), type_name(std::move(type_name)),
	  field_name(std::move(field_name)), field_number(field_number) {}
