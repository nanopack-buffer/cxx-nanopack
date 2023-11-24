#ifndef NANOPACK_NANOC_TS_GENERATOR_HXX
#define NANOPACK_NANOC_TS_GENERATOR_HXX

#include "../data_type_code_generator.hxx"
#include "../generator.hxx"

class TsGenerator final : public Generator {
  private:
	std::shared_ptr<DataTypeCodeGenerator> user_defined_message_type_generator;
	std::shared_ptr<DataTypeCodeGeneratorRegistry> data_type_generator_registry;

	std::shared_ptr<DataTypeCodeGenerator>
	find_generator_for_field(const MessageField &field) const;

  public:
	TsGenerator();

	void generate_for_schema(const MessageSchema &schema) override;
};

#endif // NANOPACK_NANOC_TS_GENERATOR_HXX