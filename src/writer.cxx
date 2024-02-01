#include <nanopack/writer.hxx>

NanoPack::Writer::Writer(std::vector<uint8_t> *buffer) : buffer(buffer) {}

void NanoPack::Writer::write_type_id(const int32_t type_id) {
	write_int32(type_id, 0);
}

void NanoPack::Writer::write_field_size(const int field_number, const int32_t size) {
	write_int32(size, sizeof(int32_t) * (field_number + 1));
}

void NanoPack::Writer::write_int32(const int32_t num, const int offset) {
	(*buffer)[offset] = num & 0x000000FF;
	(*buffer)[offset + 1] = (num & 0x0000FF00) >> 8;
	(*buffer)[offset + 2] = (num & 0x00FF0000) >> 16;
	(*buffer)[offset + 3] = (num & 0xFF000000) >> 24;
}

void NanoPack::Writer::append_int8(const int8_t num) { buffer->emplace_back(num); }

void NanoPack::Writer::append_int32(const int32_t num) {
	buffer->emplace_back(num & 0x000000FF);
	buffer->emplace_back((num & 0x0000FF00) >> 8);
	buffer->emplace_back((num & 0x00FF0000) >> 16);
	buffer->emplace_back((num & 0xFF000000) >> 24);
}

void NanoPack::Writer::append_string(const std::string &str) {
	buffer->insert(buffer->end(), str.begin(), str.end());
}

void NanoPack::Writer::append_string(const std::string_view &string_view) {
	buffer->insert(buffer->end(), string_view.begin(), string_view.end());
}

void NanoPack::Writer::append_bytes(const std::vector<uint8_t> &bytes) {
	buffer->insert(buffer->end(), bytes.begin(), bytes.end());
}

void NanoPack::Writer::append_bool(const bool b) { buffer->emplace_back(b ? 1 : 0); }

void NanoPack::Writer::append_double(double d) {
	auto *bytes = reinterpret_cast<uint8_t *>(&d);
	buffer->insert(buffer->end(), bytes, bytes + sizeof(double));
}
