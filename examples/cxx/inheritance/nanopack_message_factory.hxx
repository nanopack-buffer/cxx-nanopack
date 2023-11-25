#ifndef NANOPACK_MESSAGE_FACTORY_HXX
#define NANOPACK_MESSAGE_FACTORY_HXX

#include <memory>
#include <nanopack/message.hxx>

std::unique_ptr<NanoPack::Message>
make_nanopack_message(int32_t type_id,
                      std::vector<uint8_t>::const_iterator data_iter,
                      int &bytes_read);

#endif
