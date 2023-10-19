#ifndef NANOPACK_NANOBUF_HXX
#define NANOPACK_NANOBUF_HXX

#include <vector>

/**
 * A nanopack buffer for creating a binary nanopack message.
 */
class NanoBuf {
  private:
	std::vector<uint8_t> buf;

  public:
	/**
	 * Creates a new Nanopack buffer, with the specified amount of bytes zeroed
	 * out.
	 * @param size Number of zero bytes that should be added to the buffer.
	 */
	explicit NanoBuf(size_t size);

	/**
	 * Copies the given bytes into a new Nanopack buffer.
	 * @param buf The vector of bytes to copy from.
	 */
	explicit NanoBuf(std::vector<uint8_t> &buf);

	/**
	 * @return The number of bytes currently written to the buffer.
	 */
	size_t size();

	/**
	 * @return The backing byte array for this buffer. THIS IS NOT A COPY.
	 */
	std::vector<uint8_t> &bytes();

	int32_t read_type_id();

	int32_t read_field_size(int field_number);

	std::string read_string(int offset, int32_t size);

	int8_t read_int8(int offset);

	int32_t read_int32(int offset);

	bool read_bool(int offset);

	double read_double(int offset);

	void write_type_id(int32_t type_id);

	void write_field_size(int field_number, int32_t size);

	void append_int8(int8_t num);

	void append_int32(int32_t num);

	void write_int32(int32_t num, int offset);

	void append_string(const std::string &str);

	void append_bytes(const std::vector<uint8_t> &bytes);

	void append_bool(bool b);

	void append_double(double d);

	void print_bytes();
};

#endif // NANOPACK_NANOBUF_HXX
