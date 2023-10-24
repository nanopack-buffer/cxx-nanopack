#include "data_type_code_generator.hxx"

DataTypeCodeGeneratorRegistry::DataTypeCodeGeneratorRegistry() : generators() {}

void DataTypeCodeGeneratorRegistry::add_generator_for_type(
	NanoPack::DataType *data_type,
	std::shared_ptr<DataTypeCodeGenerator> generator) {
	add_generator_for_type(data_type->identifier(), generator);
}

void DataTypeCodeGeneratorRegistry::add_generator_for_type(
	const std::string &type_identifier,
	std::shared_ptr<DataTypeCodeGenerator> generator) {
	generators.insert({type_identifier, std::move(generator)});
}

std::shared_ptr<DataTypeCodeGenerator>
DataTypeCodeGeneratorRegistry::find_generator_for_type(
	NanoPack::DataType *data_type) {
	find_generator_for_type(data_type->identifier());
}

std::shared_ptr<DataTypeCodeGenerator>
DataTypeCodeGeneratorRegistry::find_generator_for_type(
	std::string type_identifier) {
	const auto entry = generators.find(type_identifier);
	if (entry == generators.end()) {
		return nullptr;
	}
	return entry->second;
}
