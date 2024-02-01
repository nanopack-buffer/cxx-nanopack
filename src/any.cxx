#include <nanopack/any.hxx>

NanoPack::Any::Any(int8_t i)
	: _size(1), _data{*reinterpret_cast<uint8_t *>(&i)} {}
NanoPack::Any::Any(int32_t i) : _size(4), _data(4) {
	_data[0] = i & 0x000000FF;
	_data[1] = (i & 0x0000FF00) >> 8;
	_data[2] = (i & 0x00FF0000) >> 16;
	_data[3] = (i & 0xFF000000) >> 24;
}
NanoPack::Any::Any(int64_t i) : _size(4), _data(sizeof(int64_t)) {
	_data[0] = i & 0x00000000000000FF;
	_data[1] = (i & 0x000000000000FF00) >> 8;
	_data[2] = (i & 0x0000000000FF0000) >> 16;
	_data[3] = (i & 0x00000000FF000000) >> 24;
	_data[4] = (i & 0x000000FF00000000) >> 32;
	_data[5] = (i & 0x0000FF0000000000) >> 40;
	_data[6] = (i & 0x00FF000000000000) >> 48;
	_data[7] = (i & 0xFF00000000000000) >> 56;
}

NanoPack::Any::Any(const std::string &string)
	: _size(string.size()), _data(string.begin(), string.end()) {}

NanoPack::Any::Any(const Message &message) {
	std::vector<uint8_t> data = message.data();
	_size = data.size();
	_data = std::move(data);
}

NanoPack::Any::Any(std::vector<uint8_t> data)
	: _size(data.size()), _data(std::move(data)) {}
NanoPack::Any::Any(std::vector<uint8_t>::const_iterator begin,
				   std::vector<uint8_t>::const_iterator end)
	: _data(begin, end), _size(data.size()) {}

NanoPack::Reader NanoPack::Any::as_reader() const {
	return Reader(data.begin());
}
