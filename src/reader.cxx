#include <nanopack/reader.hxx>

NanoPack::Reader::Reader(std::vector<uint8_t>::const_iterator begin)
	: buf_begin(begin) {}

std::vector<uint8_t>::const_iterator NanoPack::Reader::begin() const {
	return buf_begin;
}

int32_t NanoPack::Reader::read_type_id() const { return read_int32(0); }

int8_t NanoPack::Reader::read_int8(int offset) const {
	const uint8_t b = *(buf_begin + offset);
	const uint8_t sign_bit = b & (1 << 7);
	if (sign_bit == 0)
		return static_cast<int8_t>(b);
	return -(~b + 1);
}

int32_t NanoPack::Reader::read_int32(int offset) const {
	int32_t num = 0;
	const uint8_t last_byte = *(buf_begin + offset + 3);

	num |= *(buf_begin + offset);
	num |= *(buf_begin + offset + 1) << 8;
	num |= *(buf_begin + offset + 2) << 16;
	num |= last_byte << 24;

	const uint8_t sign_bit = last_byte & (1 << 7);
	return sign_bit == 0 ? num : -((~num) + 1);
}

int64_t NanoPack::Reader::read_int64(const int offset) const {
	int64_t num = 0;
	const uint8_t last_byte = *(buf_begin + offset + 7);

	num |= *(buf_begin + offset);
	num |= *(buf_begin + offset + 1) << 8;
	num |= *(buf_begin + offset + 2) << 16;
	num |= *(buf_begin + offset + 3) << 24;
	num |= *(buf_begin + offset + 4) << 32;
	num |= *(buf_begin + offset + 5) << 40;
	num |= *(buf_begin + offset + 6) << 48;
	num |= last_byte << 56;

	const uint8_t sign_bit = last_byte & (1 << 7);
	return sign_bit == 0 ? num : -((~num) + 1);
}

bool NanoPack::Reader::read_bool(int offset) const {
	return *(buf_begin + offset) == 1;
}

double NanoPack::Reader::read_double(int offset) const {
	return *(reinterpret_cast<const double *>(&*(buf_begin + offset)));
}

std::string NanoPack::Reader::read_string(int offset, int32_t size) const {
	return {buf_begin + offset, buf_begin + offset + size};
}

int32_t NanoPack::Reader::read_field_size(int field_number) const {
	return read_int32(sizeof(int32_t) * (field_number + 1));
}
