#include <nanopack/writer.hxx>

void NanoPack::write_type_id(TypeId type_id, int offset,
							 std::vector<uint8_t> &buf) {
	buf[offset] = type_id & 0x000000FF;
	buf[offset + 1] = (type_id & 0x0000FF00) >> 8;
	buf[offset + 2] = (type_id & 0x00FF0000) >> 16;
	buf[offset + 3] = (type_id & 0xFF000000) >> 24;
}

void NanoPack::write_field_size(const int field_number, const int32_t size,
								const int offset, std::vector<uint8_t> &buf) {
	const int p = offset + 4 * (field_number + 1);
	buf[p] = size & 0x000000FF;
	buf[p + 1] = (size & 0x0000FF00) >> 8;
	buf[p + 2] = (size & 0x00FF0000) >> 16;
	buf[p + 3] = (size & 0xFF000000) >> 24;
}

void NanoPack::write_uint32(uint32_t num, int offset,
							std::vector<uint8_t> &buf) {
	buf[offset] = num & 0x000000FF;
	buf[offset + 1] = (num & 0x0000FF00) >> 8;
	buf[offset + 2] = (num & 0x00FF0000) >> 16;
	buf[offset + 3] = (num & 0xFF000000) >> 24;
}

void NanoPack::append_int8(const int8_t num, std::vector<uint8_t> &buf) {
	buf.emplace_back(num);
}

void NanoPack::append_uint8(const uint8_t num, std::vector<uint8_t> &buf) {
	buf.emplace_back(num);
}

void NanoPack::append_int32(const int32_t num, std::vector<uint8_t> &buf) {
	buf.emplace_back(num & 0x000000FF);
	buf.emplace_back((num & 0x0000FF00) >> 8);
	buf.emplace_back((num & 0x00FF0000) >> 16);
	buf.emplace_back((num & 0xFF000000) >> 24);
}

void NanoPack::append_uint32(uint32_t num, std::vector<uint8_t> &buf) {
	buf.emplace_back(num & 0x000000FF);
	buf.emplace_back((num & 0x0000FF00) >> 8);
	buf.emplace_back((num & 0x00FF0000) >> 16);
	buf.emplace_back((num & 0xFF000000) >> 24);
}

void NanoPack::append_int64(int64_t num, std::vector<uint8_t> &buf) {
	buf.emplace_back(num & 0x00000000000000FF);
	buf.emplace_back((num & 0x000000000000FF00) >> 8);
	buf.emplace_back((num & 0x0000000000FF0000) >> 16);
	buf.emplace_back((num & 0x00000000FF000000) >> 24);
	buf.emplace_back((num & 0x000000FF00000000) >> 32);
	buf.emplace_back((num & 0x0000FF0000000000) >> 40);
	buf.emplace_back((num & 0x00FF000000000000) >> 48);
	buf.emplace_back((num & 0xFF00000000000000) >> 56);
}

void NanoPack::append_uint64(uint16_t num, std::vector<uint8_t> &buf) {
	buf.emplace_back(num & 0x00000000000000FF);
	buf.emplace_back((num & 0x000000000000FF00) >> 8);
	buf.emplace_back((num & 0x0000000000FF0000) >> 16);
	buf.emplace_back((num & 0x00000000FF000000) >> 24);
	buf.emplace_back((num & 0x000000FF00000000) >> 32);
	buf.emplace_back((num & 0x0000FF0000000000) >> 40);
	buf.emplace_back((num & 0x00FF000000000000) >> 48);
	buf.emplace_back((num & 0xFF00000000000000) >> 56);
}

void NanoPack::append_string(const std::string &str,
							 std::vector<uint8_t> &buf) {
	buf.insert(buf.end(), str.begin(), str.end());
}

void NanoPack::append_string(const std::string_view &string_view,
							 std::vector<uint8_t> &buf) {
	buf.insert(buf.end(), string_view.begin(), string_view.end());
}

void NanoPack::append_bytes(const std::vector<uint8_t> &bytes,
							std::vector<uint8_t> &buf) {
	buf.insert(buf.end(), bytes.begin(), bytes.end());
}

void NanoPack::append_bool(bool b, std::vector<uint8_t> &buf) {
	buf.emplace_back(b ? 1 : 0);
}

void NanoPack::append_double(double d, std::vector<uint8_t> &buf) {
	auto *bytes = reinterpret_cast<uint8_t *>(&d);
	buf.insert(buf.end(), bytes, bytes + sizeof(double));
}

NanoPack::Writer::Writer(std::vector<uint8_t> *buffer) : Writer(buffer, 0) {}

NanoPack::Writer::Writer(std::vector<uint8_t> *buffer, const int offset)
	: offset(offset), buffer(buffer) {}

void NanoPack::Writer::write_type_id(const TypeId type_id) {
	(*buffer)[offset] = type_id & 0x000000FF;
	(*buffer)[offset + 1] = (type_id & 0x0000FF00) >> 8;
	(*buffer)[offset + 2] = (type_id & 0x00FF0000) >> 16;
	(*buffer)[offset + 3] = (type_id & 0xFF000000) >> 24;
}

void NanoPack::Writer::write_field_size(const int field_number,
										const int32_t size) {
	write_int32(size, sizeof(int32_t) * (field_number + 1));
}

void NanoPack::Writer::write_int32(const int32_t num, const int offset) {
	const int p = offset + this->offset;
	(*buffer)[p] = num & 0x000000FF;
	(*buffer)[p + 1] = (num & 0x0000FF00) >> 8;
	(*buffer)[p + 2] = (num & 0x00FF0000) >> 16;
	(*buffer)[p + 3] = (num & 0xFF000000) >> 24;
}

void NanoPack::Writer::append_int8(const int8_t num) {
	buffer->emplace_back(num);
}
void NanoPack::Writer::append_uint8(uint8_t num) { buffer->emplace_back(num); }

void NanoPack::Writer::append_int32(const int32_t num) {
	buffer->emplace_back(num & 0x000000FF);
	buffer->emplace_back((num & 0x0000FF00) >> 8);
	buffer->emplace_back((num & 0x00FF0000) >> 16);
	buffer->emplace_back((num & 0xFF000000) >> 24);
}
void NanoPack::Writer::append_uint32(const uint32_t num) {
	buffer->emplace_back(num & 0x000000FF);
	buffer->emplace_back((num & 0x0000FF00) >> 8);
	buffer->emplace_back((num & 0x00FF0000) >> 16);
	buffer->emplace_back((num & 0xFF000000) >> 24);
}

void NanoPack::Writer::append_int64(const int64_t num) {
	buffer->emplace_back(num & 0x00000000000000FF);
	buffer->emplace_back((num & 0x000000000000FF00) >> 8);
	buffer->emplace_back((num & 0x0000000000FF0000) >> 16);
	buffer->emplace_back((num & 0x00000000FF000000) >> 24);
	buffer->emplace_back((num & 0x000000FF00000000) >> 32);
	buffer->emplace_back((num & 0x0000FF0000000000) >> 40);
	buffer->emplace_back((num & 0x00FF000000000000) >> 48);
	buffer->emplace_back((num & 0xFF00000000000000) >> 56);
}
void NanoPack::Writer::append_uint64(const uint64_t num) {
	buffer->emplace_back(num & 0x00000000000000FF);
	buffer->emplace_back((num & 0x000000000000FF00) >> 8);
	buffer->emplace_back((num & 0x0000000000FF0000) >> 16);
	buffer->emplace_back((num & 0x00000000FF000000) >> 24);
	buffer->emplace_back((num & 0x000000FF00000000) >> 32);
	buffer->emplace_back((num & 0x0000FF0000000000) >> 40);
	buffer->emplace_back((num & 0x00FF000000000000) >> 48);
	buffer->emplace_back((num & 0xFF00000000000000) >> 56);
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

void NanoPack::Writer::append_bool(const bool b) {
	buffer->emplace_back(b ? 1 : 0);
}

void NanoPack::Writer::append_double(double d) {
	auto *bytes = reinterpret_cast<uint8_t *>(&d);
	buffer->insert(buffer->end(), bytes, bytes + sizeof(double));
}
