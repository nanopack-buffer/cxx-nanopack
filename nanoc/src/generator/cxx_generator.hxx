#ifndef NANOPACK_NANOC_CXX_GENERATOR_HXX
#define NANOPACK_NANOC_CXX_GENERATOR_HXX

#include "../message_schema.hxx"
#include <string>

class CxxGenerator {
  private:
	std::string generate_header_file(const MessageSchema &schema);

	void generate_code_file(const MessageSchema &schema,
							const std::string &header_file_name);

  public:
	CxxGenerator();

	void generate_for_schema(const MessageSchema &schema);
};

#endif // NANOPACK_NANOC_CXX_GENERATOR_HXX
