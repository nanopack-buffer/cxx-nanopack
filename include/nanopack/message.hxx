#ifndef NANOPACK_MESSAGE_HXX
#define NANOPACK_MESSAGE_HXX

#include <cstdint>
#include <vector>

#include "nanopack.hxx"

namespace NanoPack {

class Message {
  public:
	virtual ~Message() = default;

	[[nodiscard]] virtual TypeId type_id() const = 0;

	[[nodiscard]] virtual int header_size() const = 0;

	virtual void write_to(std::vector<uint8_t> &buf, int offset) const = 0;

	[[nodiscard]] virtual std::vector<uint8_t> data() const = 0;
};

} // namespace NanoPack

#endif // NANOPACK_MESSAGE_HXX
