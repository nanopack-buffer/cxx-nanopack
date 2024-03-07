#ifndef NANOPACK_WRITER_HXX
#define NANOPACK_WRITER_HXX

#include <cstdint>
#include <string>
#include <vector>

#include "nanopack.hxx"

namespace NanoPack {

void write_type_id(TypeId type_id, int offset, std::vector<uint8_t> &buf);

void write_field_size(int field_number, int32_t size, int offset,
					  std::vector<uint8_t> &buf);

void append_int8(int8_t num, std::vector<uint8_t> &buf);
void append_uint8(uint8_t num, std::vector<uint8_t> &buf);

void append_int32(int32_t num, std::vector<uint8_t> &buf);
void append_uint32(uint32_t num, std::vector<uint8_t> &buf);

void append_int64(int64_t num, std::vector<uint8_t> &buf);
void append_uint64(uint16_t num, std::vector<uint8_t> &buf);

void append_string(const std::string &str, std::vector<uint8_t> &buf);
void append_string(const std::string_view &string_view,
				   std::vector<uint8_t> &buf);

void append_bytes(const std::vector<uint8_t> &bytes, std::vector<uint8_t> &buf);

void append_bool(bool b, std::vector<uint8_t> &buf);

void append_double(double d, std::vector<uint8_t> &buf);

class Writer {
  private:
	int offset;
	std::vector<uint8_t> *buffer;

  public:
	explicit Writer(std::vector<uint8_t> *buffer);

	Writer(std::vector<uint8_t> *buffer, int offset);

	void write_type_id(TypeId type_id);

	void write_field_size(int field_number, int32_t size);

	void append_int8(int8_t num);
	void append_uint8(uint8_t num);

	void append_int32(int32_t num);
	void append_uint32(uint32_t num);

	void append_int64(int64_t num);
	void append_uint64(uint64_t num);

	void write_int32(int32_t num, int offset);

	void append_string(const std::string &str);
	void append_string(const std::string_view &string_view);

	void append_bytes(const std::vector<uint8_t> &bytes);

	void append_bool(bool b);

	void append_double(double d);
};

} // namespace NanoPack

#endif // NANOPACK_WRITER_HXX
