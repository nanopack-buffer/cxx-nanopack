#include "np_optional.hxx"

NanoPack::Optional::Optional(std::shared_ptr<DataType> value_type)
	: value_type(std::move(value_type)) {}

std::string NanoPack::Optional::identifier() {
	return NanoPack::Optional::IDENTIFIER;
}

bool NanoPack::Optional::is_fixed_size() { return false; }

NanoPack::Size NanoPack::Optional::size() {
	return NanoPack::DataType::DYNAMIC_SIZE;
}

std::shared_ptr<NanoPack::DataType> NanoPack::Optional::get_value_type() {
	return value_type;
}
