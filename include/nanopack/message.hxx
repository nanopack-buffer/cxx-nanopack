#ifndef NANOPACK_MESSAGE_HXX
#define NANOPACK_MESSAGE_HXX

#include <vector>

namespace NanoPack {

class Message {
  public:
	[[nodiscard]] virtual std::vector<uint8_t> data() const = 0;

	virtual ~Message() = default;
};

} // namespace NanoPack

#endif // NANOPACK_MESSAGE_HXX
