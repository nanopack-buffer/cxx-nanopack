#ifndef NANOPACK_READER_HXX
#define NANOPACK_READER_HXX

#include <cstdint>
#include <string>
#include <vector>

namespace NanoPack {

class Reader {
  private:
	std::vector<uint8_t>::const_iterator buf_begin;
	std::vector<uint8_t>::const_iterator buf_end;

  public:
	explicit Reader(std::vector<uint8_t>::const_iterator begin);

	[[nodiscard]] std::vector<uint8_t>::const_iterator begin() const;

	[[nodiscard]] int32_t read_type_id() const;

	[[nodiscard]] int32_t read_field_size(int field_number) const;

	[[nodiscard]] std::string read_string(int offset, int32_t size) const;

	[[nodiscard]] int8_t read_int8(int offset) const;

	[[nodiscard]] int32_t read_int32(int offset) const;

	[[nodiscard]] bool read_bool(int offset) const;

	[[nodiscard]] double read_double(int offset) const;
};

} // namespace NanoPack

#endif // NANOPACK_READER_HXX
