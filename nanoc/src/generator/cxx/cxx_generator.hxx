#ifndef NANOPACK_NANOC_CXX_GENERATOR_HXX
#define NANOPACK_NANOC_CXX_GENERATOR_HXX

#include "../../message_schema.hxx"
#include "../data_type_code_generator.hxx"
#include <map>
#include <string>

class CxxGenerator {
  private:
	std::shared_ptr<DataTypeCodeGeneratorRegistry> data_type_generator_registry;

	std::map<std::string, DataTypeCodeGenerator *> data_type_generators;

	std::shared_ptr<DataTypeCodeGenerator>
	find_generator_for_field(const MessageField &field);

	std::string generate_header_file(const MessageSchema &schema);

	void generate_code_file(const MessageSchema &schema,
							const std::string &header_file_name);

	void format_code_file(const std::filesystem::path &path);

  public:
	CxxGenerator();
	~CxxGenerator();

	void generate_for_schema(const MessageSchema &schema);
};

#endif // NANOPACK_NANOC_CXX_GENERATOR_HXX