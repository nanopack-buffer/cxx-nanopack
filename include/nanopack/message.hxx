#ifndef NANOPACK_MESSAGE_HXX
#define NANOPACK_MESSAGE_HXX

#include "nanopack.hxx"
#include "nanopack/writer.hxx"

namespace NanoPack {

class Message {
  public:
	virtual ~Message() = default;

	[[nodiscard]] virtual TypeId type_id() const = 0;

	[[nodiscard]] virtual size_t header_size() const = 0;

	virtual size_t write_to(Writer &writer, int offset) const = 0;
};

} // namespace NanoPack

#endif // NANOPACK_MESSAGE_HXX
