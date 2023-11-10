#include "np_map.hxx"

NanoPack::Map::Map(std::shared_ptr<DataType> key_type,
				   std::shared_ptr<DataType> value_type)
	: key_type(std::move(key_type)), value_type(std::move(value_type)) {}

std::string NanoPack::Map::identifier() { return NanoPack::Map::IDENTIFIER; }

bool NanoPack::Map::is_fixed_size() { return false; }

bool NanoPack::Map::is_user_defined() { return false; }

NanoPack::Size NanoPack::Map::size() {
	return NanoPack::DataType::DYNAMIC_SIZE;
}

std::shared_ptr<NanoPack::DataType> NanoPack::Map::get_key_type() {
	return key_type;
}

std::shared_ptr<NanoPack::DataType> NanoPack::Map::get_value_type() {
	return value_type;
}
