#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <nanopack/writer.hxx>
#include <string>
#include <string_view>

NanoPack::Writer::Writer() : is_owning(true), end_ptr(0), capacity(500) {
	buf = (uint8_t *)(std::malloc(capacity * sizeof(uint8_t)));
}
NanoPack::Writer::Writer(size_t initial_size)
	: is_owning(true), end_ptr(0), capacity(initial_size) {
	buf = (uint8_t *)(std::malloc(initial_size * sizeof(uint8_t)));
}
NanoPack::Writer::Writer(uint8_t *dest, size_t initial_size)
	: buf(dest), is_owning(false), capacity(initial_size), end_ptr(0) {}

NanoPack::Writer::~Writer() {
	if (is_owning && buf != nullptr) {
		free(buf);
	}
}

uint8_t *NanoPack::Writer::data() { return buf; }
uint8_t *NanoPack::Writer::into_data() {
	const auto p = buf;
	buf = nullptr;
	return p;
}

size_t NanoPack::Writer::size() { return end_ptr; }

void NanoPack::Writer::reserve_header(size_t header_size) {
	end_ptr += header_size;
}

void NanoPack::Writer::write_type_id(uint32_t type_id, int offset) {
	buf[offset] = type_id & 0x000000FF;
	buf[offset + 1] = (type_id & 0x0000FF00) >> 8;
	buf[offset + 2] = (type_id & 0x00FF0000) >> 16;
	buf[offset + 3] = (type_id & 0xFF000000) >> 24;
}

void NanoPack::Writer::write_field_size(int field_number, int32_t size,
										int offset) {
	const int p = offset + 4 * (field_number + 1);
	buf[p] = size & 0x000000FF;
	buf[p + 1] = (size & 0x0000FF00) >> 8;
	buf[p + 2] = (size & 0x00FF0000) >> 16;
	buf[p + 3] = (size & 0xFF000000) >> 24;
}

void NanoPack::Writer::append_int8(int8_t num) {
	move_end_ptr(sizeof(int8_t));
	buf[end_ptr - 1] = num;
}
void NanoPack::Writer::append_uint8(uint8_t num) {
	move_end_ptr(sizeof(uint8_t));
	buf[end_ptr - 1] = num;
}

void NanoPack::Writer::append_int32(int32_t num) {
	move_end_ptr(sizeof(int32_t));
	buf[end_ptr - 4] = num & 0x000000FF;
	buf[end_ptr - 3] = (num & 0x0000FF00) >> 8;
	buf[end_ptr - 2] = (num & 0x00FF0000) >> 16;
	buf[end_ptr - 1] = (num & 0xFF000000) >> 24;
}
void NanoPack::Writer::append_uint32(uint32_t num) {
	move_end_ptr(sizeof(uint32_t));
	buf[end_ptr - 4] = num & 0x000000FF;
	buf[end_ptr - 3] = (num & 0x0000FF00) >> 8;
	buf[end_ptr - 2] = (num & 0x00FF0000) >> 16;
	buf[end_ptr - 1] = (num & 0xFF000000) >> 24;
}

void NanoPack::Writer::append_int64(int64_t num) {
	move_end_ptr(sizeof(int64_t));
	buf[end_ptr - 8] = num & 0x00000000000000FF;
	buf[end_ptr - 7] = (num & 0x000000000000FF00) >> 8;
	buf[end_ptr - 6] = (num & 0x0000000000FF0000) >> 16;
	buf[end_ptr - 5] = (num & 0x00000000FF000000) >> 24;
	buf[end_ptr - 4] = (num & 0x000000FF00000000) >> 32;
	buf[end_ptr - 3] = (num & 0x0000FF0000000000) >> 40;
	buf[end_ptr - 2] = (num & 0x00FF000000000000) >> 48;
	buf[end_ptr - 1] = (num & 0xFF00000000000000) >> 56;
}

void NanoPack::Writer::append_uint64(uint64_t num) {
	move_end_ptr(sizeof(int64_t));
	buf[end_ptr - 8] = num & 0x00000000000000FF;
	buf[end_ptr - 7] = (num & 0x000000000000FF00) >> 8;
	buf[end_ptr - 6] = (num & 0x0000000000FF0000) >> 16;
	buf[end_ptr - 5] = (num & 0x00000000FF000000) >> 24;
	buf[end_ptr - 4] = (num & 0x000000FF00000000) >> 32;
	buf[end_ptr - 3] = (num & 0x0000FF0000000000) >> 40;
	buf[end_ptr - 2] = (num & 0x00FF000000000000) >> 48;
	buf[end_ptr - 1] = (num & 0xFF00000000000000) >> 56;
}

void NanoPack::Writer::append_bool(bool b) {
	move_end_ptr(1);
	buf[end_ptr - 1] = b ? 1 : 0;
}

void NanoPack::Writer::append_double(double d) {
	const size_t start = end_ptr;
	move_end_ptr(sizeof(double));
	std::memcpy(buf + start, &d, sizeof(double));
}

void NanoPack::Writer::append_string(const std::string &str) {
	const size_t start = end_ptr;
	const size_t len = str.size();
	move_end_ptr(len);
	std::memcpy(buf + start, str.data(), len);
}
void NanoPack::Writer::append_string(const std::string_view &str) {
	const size_t start = end_ptr;
	const size_t len = str.size();
	move_end_ptr(len);
	std::memcpy(buf + start, str.data(), len);
}

void NanoPack::Writer::append_bytes(const uint8_t *bytes, size_t size) {
	const size_t start = end_ptr;
	move_end_ptr(size);
	std::memcpy(buf + start, bytes, size);
}

void NanoPack::Writer::move_end_ptr(size_t by) {
	end_ptr += by;
	if (end_ptr >= capacity) {
		buf = (uint8_t *)std::realloc(buf, capacity * 2 + (end_ptr - capacity));
	}
}

void NanoPack::Writer::reset() { end_ptr = 0; }
