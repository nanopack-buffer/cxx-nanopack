#ifndef NANOPACK_NANOC_RESOLVE_SCHEMAS_HXX
#define NANOPACK_NANOC_RESOLVE_SCHEMAS_HXX

#include "../message_schema.hxx"
#include "../parser/parse_schema.hxx"
#include <oneapi/tbb/concurrent_unordered_map.h>

std::vector<MessageSchema> resolve_schemas(
	const oneapi::tbb::concurrent_unordered_map<std::string, ParseResult>
		&parse_results);

#endif // NANOPACK_NANOC_RESOLVE_SCHEMAS_HXX
