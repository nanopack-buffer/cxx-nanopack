#ifndef NANOPACK_ANY_HXX
#define NANOPACK_ANY_HXX

#include "message.hxx"
#include "reader.hxx"

#include <vector>

namespace NanoPack {

class Any {
  private:
	size_t _size;
	std::vector<uint8_t> _data;

  public:
	const size_t &size = _size;
	const std::vector<uint8_t> &data = _data;

	Any(const Any &other);
	Any(Any &&other) noexcept;

	Any(int8_t i);
	Any(int32_t i);
	Any(int64_t i);

	Any(const std::string &string);

	Any(const Message &message);

	Any(std::vector<uint8_t> data);
	Any(std::vector<uint8_t>::const_iterator start,
		std::vector<uint8_t>::const_iterator end);

	[[nodiscard]] Reader as_reader() const;
};

} // namespace NanoPack

#endif // NANOPACK_ANY_HXX
