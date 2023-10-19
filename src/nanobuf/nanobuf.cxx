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
	return static_cast<int8_t>(buf.at(offset));
}

int32_t NanoBuf::read_int32(int offset) {
	int32_t num = 0;
	num |= static_cast<uint8_t>(buf.at(offset));
	num |= static_cast<uint8_t>(buf.at(offset + 1) << 8);
	num |= static_cast<uint8_t>(buf.at(offset + 2) << 16);
	num |= static_cast<uint8_t>(buf.at(offset + 3) << 24);
	return num;
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
