#include <nanopack/any.hxx>

NanoPack::Any::Any(int8_t i)
	: _size(1), _data{*reinterpret_cast<uint8_t *>(&i)} {}
NanoPack::Any::Any(int32_t i)
	: _size(4), _data(reinterpret_cast<uint8_t *>(&i),
					  reinterpret_cast<uint8_t *>(&i) + sizeof(int32_t)) {}
NanoPack::Any::Any(int64_t i)
	: _size(4), _data(reinterpret_cast<uint8_t *>(&i),
					  reinterpret_cast<uint8_t *>(&i) + sizeof(int64_t)) {}

NanoPack::Any::Any(const std::string &string)
	: _size(string.size()), _data(string.begin(), string.end()) {}

NanoPack::Any::Any(const Message &message) {
	std::vector<uint8_t> data = message.data();
	_size = data.size();
	_data = std::move(data);
}
NanoPack::Any::Any(std::vector<uint8_t> data)
	: _size(data.size()), _data(std::move(data)) {}

NanoPack::Reader NanoPack::Any::as_reader() const {
	return Reader(data.begin());
}
