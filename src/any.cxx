#include "nanopack/writer.hxx"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <nanopack/any.hxx>

NanoPack::Any::Any() : _size(0) {}

NanoPack::Any::~Any() {
	if (buf != nullptr) {
		free(buf);
	}
}

NanoPack::Any::Any(const Any &other) : _size(other._size) {
	buf = (uint8_t *)std::malloc(_size);
	std::memcpy(buf, other.buf, _size);
}
NanoPack::Any &NanoPack::Any::operator=(const Any &other) {
	_size = other._size;
	buf = (uint8_t *)std::malloc(_size);
	std::memcpy(buf, other.buf, _size);
	return *this;
}

NanoPack::Any::Any(Any &&other) noexcept : _size(other._size) {
	buf = other.buf;
	other.buf = nullptr;
}
NanoPack::Any &NanoPack::Any::operator=(Any &&other) noexcept {
	_size = other._size;
	buf = other.buf;
	other.buf = nullptr;
	return *this;
}

NanoPack::Any::Any(int8_t i) : _size(1) { memcpy(buf, &i, sizeof(int8_t)); }
NanoPack::Any::Any(int32_t i) : _size(4) {
	buf = (uint8_t *)std::malloc(sizeof(int32_t));
	buf[0] = i & 0x000000FF;
	buf[1] = (i & 0x0000FF00) >> 8;
	buf[2] = (i & 0x00FF0000) >> 16;
	buf[3] = (i & 0xFF000000) >> 24;
}
NanoPack::Any::Any(int64_t i) : _size(4) {
	buf = (uint8_t *)std::malloc(sizeof(int64_t));
	buf[0] = i & 0x00000000000000FF;
	buf[1] = (i & 0x000000000000FF00) >> 8;
	buf[2] = (i & 0x0000000000FF0000) >> 16;
	buf[3] = (i & 0x00000000FF000000) >> 24;
	buf[4] = (i & 0x000000FF00000000) >> 32;
	buf[5] = (i & 0x0000FF0000000000) >> 40;
	buf[6] = (i & 0x00FF000000000000) >> 48;
	buf[7] = (i & 0xFF00000000000000) >> 56;
}

NanoPack::Any::Any(const std::string &string) : _size(string.size()) {
	buf = (uint8_t *)std::malloc(_size * sizeof(uint8_t));
	std::memcpy(buf, string.data(), _size);
}

NanoPack::Any::Any(const Message &message) {
	buf = (uint8_t *)std::malloc(100 * sizeof(uint8_t));
	Writer writer(buf, 100);
	message.write_to(writer, 0);
	_size = writer.size();
	buf = writer.data();
}

NanoPack::Any::Any(uint8_t *data, size_t size) {
	_size = size;
	buf = (uint8_t *)std::malloc(size * sizeof(uint8_t));
	std::memcpy(buf, data, size);
}

size_t NanoPack::Any::size() const { return _size; }

uint8_t *NanoPack::Any::data() const { return buf; }

NanoPack::Reader NanoPack::Any::as_reader() const { return Reader(buf); }

NanoPack::Reader NanoPack::Any::into_reader() {
	Reader reader(buf);
	buf = nullptr;
	return reader;
}
