#ifndef NANOPACK__ANY_HXX
#define NANOPACK__ANY_HXX

#include <cstdint>

#include "message.hxx"
#include "reader.hxx"

namespace NanoPack {

class Any {
	size_t _size;
	uint8_t *buf;

  public:
	Any();
	~Any();

	Any(const Any &other);
	Any &operator=(const Any &other);

	Any(Any &&other) noexcept;
	Any &operator=(Any &&other) noexcept;

	Any(int8_t i);
	Any(int32_t i);
	Any(int64_t i);

	Any(const std::string &string);

	Any(const Message &message);

	Any(uint8_t *data, size_t size);

	[[nodiscard]] size_t size() const;

	[[nodiscard]] uint8_t *data() const;

	[[nodiscard]] Reader as_reader() const;

	[[nodiscard]] Reader into_reader();
};

} // namespace NanoPack

#endif // NANOPACK__ANY_HXX
