#ifndef NANOPACK_NANOC_PARSE_SCHEMA_HXX
#define NANOPACK_NANOC_PARSE_SCHEMA_HXX

#include "../message_schema.hxx"
#include <optional>
#include <string>

/**
 * Parses a NanoPack schema file into MessageSchema.
 *
 * @param file_path The path to the schema file to be parsed
 * @return MessageSchema if the schema file is parsed successfully, std::nullopt
 * otherwise.
 */
std::optional<MessageSchema> parse_schema_file(const std::string &file_path);

#endif // NANOPACK_NANOC_PARSE_SCHEMA_HXX
