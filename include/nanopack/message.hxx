#ifndef NANOPACK_MESSAGE_HXX
#define NANOPACK_MESSAGE_HXX

#include <cstdint>
#include <vector>

namespace NanoPack {

class Message {
  public:
	virtual ~Message() = default;

	[[nodiscard]] virtual int32_t type_id() const = 0;

	[[nodiscard]] virtual std::vector<uint8_t> data() const = 0;
};

} // namespace NanoPack

#endif // NANOPACK_MESSAGE_HXX
