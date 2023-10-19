#ifndef NANOPACK_NANOC_PARSE_SCHEMA_HXX
#define NANOPACK_NANOC_PARSE_SCHEMA_HXX

#include "../message_schema.hxx"
#include <optional>
#include <string>

std::optional<MessageSchema> parse_schema_file(const std::string &file_path);

#endif // NANOPACK_NANOC_PARSE_SCHEMA_HXX
