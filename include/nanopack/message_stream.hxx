#ifndef NANOPACK_MESSAGE_STREAM_HXX
#define NANOPACK_MESSAGE_STREAM_HXX

#include <cstdint>
#include <vector>

namespace NanoPack {

class MessageStream {
  public:
	std::vector<uint8_t> read_message_data();
};

} // namespace NanoPack

#endif // NANOPACK_MESSAGE_STREAM_HXX
