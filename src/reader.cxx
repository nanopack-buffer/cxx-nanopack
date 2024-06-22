#include "nanopack/nanopack.hxx"
#include <cstdint>
#include <cstring>
#include <nanopack/reader.hxx>
#include <optional>

NanoPack::Reader::Reader(uint8_t *buf) : buffer(buf) {}

uint8_t *NanoPack::Reader::data() const { return buffer; }

NanoPack::TypeId NanoPack::Reader::read_type_id() const {
	TypeId id;
	read_uint32(0, id);
	return id;
}

void NanoPack::Reader::read_int8(int offset, int8_t &dest) const {
	dest = *(buffer + offset);
}

void NanoPack::Reader::read_uint8(int offset, uint8_t &dest) const {
	dest = *(buffer + offset);
}

void NanoPack::Reader::read_uint32(int offset, uint32_t &dest) const {
	dest = 0;
	dest |= *(buffer + offset);
	dest |= *(buffer + offset + 1) << 8;
	dest |= *(buffer + offset + 2) << 16;
	dest |= *(buffer + offset + 3) << 24;
}

void NanoPack::Reader::read_uint64(int offset, uint64_t &dest) const {
	dest = 0;
	dest |= buffer[offset];
	dest |= buffer[offset + 1] << 8;
	dest |= buffer[offset + 2] << 16;
	dest |= buffer[offset + 3] << 24;
	dest |= buffer[offset + 4] << 32;
	dest |= buffer[offset + 5] << 40;
	dest |= buffer[offset + 6] << 48;
	dest |= buffer[offset + 7] << 56;
}

void NanoPack::Reader::read_int32(int offset, int32_t &dest) const {
	dest = 0;
	dest |= *(buffer + offset);
	dest |= *(buffer + offset + 1) << 8;
	dest |= *(buffer + offset + 2) << 16;
	dest |= *(buffer + offset + 3) << 24;
}

void NanoPack::Reader::read_int64(int offset, int64_t &dest) const {
	dest = 0;
	dest |= *(buffer + offset);
	dest |= *(buffer + offset + 1) << 8;
	dest |= *(buffer + offset + 2) << 16;
	dest |= *(buffer + offset + 3) << 24;
	dest |= *(buffer + offset + 4) << 32;
	dest |= *(buffer + offset + 5) << 40;
	dest |= *(buffer + offset + 6) << 48;
	dest |= *(buffer + offset + 7) << 56;
}

void NanoPack::Reader::read_bool(int offset, bool &dest) const {
	dest = *(buffer + offset) == 1;
}

void NanoPack::Reader::read_double(int offset, double &dest) const {
	std::memcpy(&dest, buffer + offset, sizeof(double));
}

void NanoPack::Reader::read_string(int offset, int32_t size,
								   std::string &dest) const {
	dest.resize(size);
	std::memcpy(dest.data(), buffer + offset, size);
}
void NanoPack::Reader::read_string(int offset, int32_t size,
								   std::optional<std::string> &dest) const {
	dest = std::move(std::string());
	dest->reserve(size);
	std::memcpy(dest->data(), buffer + offset, size);
}

int32_t NanoPack::Reader::read_field_size(int field_number) const {
	int32_t i;
	read_int32(sizeof(int32_t) * (field_number + 1), i);
	return i;
}
