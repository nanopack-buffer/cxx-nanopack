#include "swift_generator.hxx"

SwiftGenerator::SwiftGenerator() : data_type_generator_registry() {
	data_type_generator_registry =
		std::make_shared<DataTypeCodeGeneratorRegistry>();
}

void SwiftGenerator::generate_for_schema(const MessageSchema &schema) {

}
