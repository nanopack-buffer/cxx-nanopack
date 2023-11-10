#ifndef NANOPACK_NANOC_PARSE_RESULT_HXX
#define NANOPACK_NANOC_PARSE_RESULT_HXX

#include "../data_type/data_type.hxx"
#include "../message_schema.hxx"
#include <oneapi/tbb/concurrent_unordered_map.h>

class ParseResult {
  private:
	oneapi::tbb::concurrent_unordered_map<std::string, MessageSchema> schemas;
	oneapi::tbb::concurrent_unordered_map<std::string, NanoPack::DataType>
		unresolved_types;

  public:
	ParseResult();

	const MessageSchema &find_message_schema(const std::string &message_name);
};

#endif // NANOPACK_NANOC_PARSE_RESULT_HXX
