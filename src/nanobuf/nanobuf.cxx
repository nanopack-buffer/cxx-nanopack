#include <iostream>
#include <nanopack/nanobuf.hxx>
#include <string>

NanoBuf::NanoBuf(size_t size) : buf(size) { buf.reserve(1024); }

NanoBuf::NanoBuf(std::vector<uint8_t> &buf) : buf(buf) {}

size_t NanoBuf::size() { return buf.size(); }

std::vector<uint8_t> &NanoBuf::bytes() { return buf; }

int32_t NanoBuf::read_type_id() { return read_int32(0); }

int32_t NanoBuf::read_field_size(int field_number) {
	return read_int32(sizeof(int32_t) * (field_number + 1));
}

std::string NanoBuf::read_string(int offset, int32_t size) {
	return {buf.begin() + offset, buf.begin() + offset + size};
}

int8_t NanoBuf::read_int8(int offset) {
	const uint8_t b = buf.at(offset);
	const uint8_t sign_bit = b & (1 << 7);
	const bool number_is_positive = sign_bit == 0;
	if (number_is_positive)
		return static_cast<int8_t>(b);
	return -((~b) + 1);
}

int32_t NanoBuf::read_int32(int offset) {
	int32_t num = 0;
	if constexpr (std::endian::native == std::endian::big) {
		num |= static_cast<uint8_t>(buf.at(offset)) << 24;
		num |= static_cast<uint8_t>(buf.at(offset + 1)) << 16;
		num |= static_cast<uint8_t>(buf.at(offset + 2)) << 8;
		num |= static_cast<uint8_t>(buf.at(offset + 3));
	} else if constexpr (std::endian::native == std::endian::little) {
		num |= static_cast<uint8_t>(buf.at(offset));
		num |= static_cast<uint8_t>(buf.at(offset + 1)) << 8;
		num |= static_cast<uint8_t>(buf.at(offset + 2)) << 16;
		num |= static_cast<uint8_t>(buf.at(offset + 3)) << 24;
	} else {
		throw "host has unsupported endianness.";
	}

	const uint8_t last_byte = buf.at(offset + 3);
	const uint8_t sign_bit = last_byte & (1 << 7);
	const bool number_is_positive = sign_bit == 0;
	if (number_is_positive)
		return num;
	return -((~num) + 1);
}

bool NanoBuf::read_bool(int offset) { return buf.at(offset) == 1; }

double NanoBuf::read_double(int offset) {
	uint8_t *data = buf.data();
	return *(reinterpret_cast<double *>(data[offset]));
}

void NanoBuf::write_type_id(int32_t type_id) { write_int32(type_id, 0); }

void NanoBuf::write_field_size(int field_number, int32_t size) {
	write_int32(size, sizeof(int32_t) * (field_number + 1));
}

void NanoBuf::write_int32(int32_t num, int offset) {
	buf[offset] = num & 0x000000FF;
	buf[offset + 1] = (num & 0x0000FF00) >> 8;
	buf[offset + 2] = (num & 0x00FF0000) >> 16;
	buf[offset + 3] = (num & 0xFF000000) >> 24;
}

void NanoBuf::append_int8(int8_t num) { buf.emplace_back(num); }

void NanoBuf::append_int32(int32_t num) {
	buf.emplace_back(num & 0x000000FF);
	buf.emplace_back((num & 0x0000FF00) >> 8);
	buf.emplace_back((num & 0x00FF0000) >> 16);
	buf.emplace_back((num & 0xFF000000) >> 24);
}

void NanoBuf::append_string(const std::string &str) {
	buf.insert(buf.end(), str.begin(), str.end());
}

void NanoBuf::append_bytes(const std::vector<uint8_t> &bytes) {
	buf.insert(buf.end(), bytes.begin(), bytes.end());
}

void NanoBuf::append_bool(bool b) { buf.emplace_back(b ? 1 : 0); }

void NanoBuf::append_double(double d) {
	auto *bytes = reinterpret_cast<uint8_t *>(&d);
	buf.insert(buf.end(), bytes, bytes + sizeof(double));
}

void NanoBuf::print_bytes() {
	for (uint8_t b : buf) {
		std::cout << +b << " ";
	}
	std::cout << std::endl;
}

uint8_t NanoBuf::operator[](int i) const { return buf[i]; }
