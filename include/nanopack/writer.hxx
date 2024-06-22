#ifndef NANOPACK__BUFFER_HXX
#define NANOPACK__BUFFER_HXX

#include "nanopack/nanopack.hxx"
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace NanoPack {

/**
 * Provides methods for serializing a NanoPack message to a buffer.
 */
class Writer {
	uint8_t *buf;
	bool is_owning;
	size_t capacity;
	size_t end_ptr;

	void move_end_ptr(size_t by);

  public:
	Writer();

	/**
	 * Creates an instance of Writer that writes to its own internal buffer the
	 * initial size of which is specified by initial_size. The buffer is freed
	 * when it is destroyed.
	 */
	Writer(size_t initial_size);

	/**
	 * Creates an instance of Writer that writes to the provided buffer.
	 * The buffer must be malloc-ed beforehand, and you are responsible for
	 * free-ing it after use. The provided buffer must also have some capacity.
	 * The capacity (initial_size) must be passed to this ctor.
	 *
	 * You can offset the buffer so that Writer starts writing from that
	 * position, in which case initial_size specifies the space available after
	 * that offset.
	 */
	Writer(uint8_t *dest, size_t initial_size);

	~Writer();

	/**
	 * Returns the internal buffer. If Writer is managing the buffer,
	 * attempting to access the pointer after it is destroyed is a UB (use
	 * after free).
	 */
	uint8_t *data();

	/**
	 * Returns the internal buffer and relinquishes Writer's ownership of it to
	 * the caller. The buffer will not be freed after Writer is destroyed.
	 */
	uint8_t *into_data();

	size_t size();

	void reserve_header(size_t header_size);

	void write_type_id(TypeId type_id, int offset);

	void write_field_size(int field_number, int32_t size, int offset);

	void append_int8(int8_t num);
	void append_uint8(uint8_t num);

	void append_int32(int32_t num);
	void append_uint32(uint32_t num);

	void append_int64(int64_t num);
	void append_uint64(uint64_t num);

	void append_bool(bool b);

	void append_double(double d);

	void append_string(const std::string &str);
	void append_string(const std::string_view &str);

	void append_bytes(const uint8_t *bytes, size_t size);

	/**
	 * Resets written content and writer's internal state. This operation does
	 * not actually clear the buffer but allows the writer to overwrite
	 * previously written content. Particularly useful when this writer is
	 * reused for serializing different messages
	 */
	void reset();
};

} // namespace NanoPack

#endif
