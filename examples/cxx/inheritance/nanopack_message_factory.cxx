#include "nanopack_message_factory.hxx"
#include "text.np.hxx"
#include "widget.np.hxx"

std::unique_ptr<NanoPack::Message>
make_nanopack_message(const int32_t type_id,
                      std::vector<uint8_t>::const_iterator data_iter,
                      int &bytes_read) {
  switch (type_id) {
  case 1:
    return std::make_unique<Widget>(data_iter, bytes_read);
  case 2:
    return std::make_unique<Text>(data_iter, bytes_read);
  default:
    return nullptr;
  }
}
