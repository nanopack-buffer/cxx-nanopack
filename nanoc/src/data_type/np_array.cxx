#include "np_array.hxx"

NanoPack::Array::Array(std::shared_ptr<DataType> item_type)
	: item_type(std::move(item_type)) {}

std::string NanoPack::Array::identifier() { return IDENTIFIER; }

bool NanoPack::Array::is_fixed_size() { return false; }

NanoPack::Size NanoPack::Array::size() { return DYNAMIC_SIZE; }

std::shared_ptr<NanoPack::DataType> NanoPack::Array::get_item_type() {
	return item_type;
}
