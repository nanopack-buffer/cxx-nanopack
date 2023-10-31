#ifndef NANOPACK_NANOC_SWIFT_GENERATOR_HXX
#define NANOPACK_NANOC_SWIFT_GENERATOR_HXX

#include "../generator.hxx"

class SwiftGenerator : public Generator {
  private:
	std::shared_ptr<DataTypeCodeGeneratorRegistry> data_type_generator_registry;

  public:
	SwiftGenerator();

	void generate_for_schema(const MessageSchema &schema) override;
};

#endif // NANOPACK_NANOC_SWIFT_GENERATOR_HXX
