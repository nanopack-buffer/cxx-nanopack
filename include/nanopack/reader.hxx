#ifndef NANOPACK__READER_HXX
#define NANOPACK__READER_HXX

#include <cstdint>
#include <optional>
#include <string>

#include "nanopack.hxx"

namespace NanoPack {

/**
 * Provides methods for reading a NanoPack-formatted buffer.
 * Reader does not take ownership of the passed buffer,
 * so you are responsible for managing it.
 */
class Reader {
  public:
	uint8_t *buffer;

	explicit Reader(uint8_t *buf);

	void set_buffer(uint8_t *buf);

	[[nodiscard]] uint8_t *data() const;

	[[nodiscard]] TypeId read_type_id() const;

	[[nodiscard]] int32_t read_field_size(int field_number) const;

	void read_string(int offset, int32_t size, std::string &dest) const;
	void read_string(int offset, int32_t size,
					 std::optional<std::string> &dest) const;

	void read_int8(int offset, int8_t &dest) const;
	void read_uint8(int offset, uint8_t &dest) const;

	void read_int32(int offset, int32_t &dest) const;
	void read_uint32(int offset, uint32_t &dest) const;

	void read_int64(int offset, int64_t &dest) const;
	void read_uint64(int offset, uint64_t &dest) const;

	void read_bool(int offset, bool &dest) const;

	void read_double(int offset, double &dest) const;
};

} // namespace NanoPack

#endif // NANOPACK__READER_HXX
