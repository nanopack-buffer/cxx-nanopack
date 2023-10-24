#ifndef NANOPACK_NANOC_TYPE_FACTORY_HXX
#define NANOPACK_NANOC_TYPE_FACTORY_HXX

#include "data_type.hxx"
#include "np_bool.hxx"
#include "np_double.hxx"
#include "np_int32.hxx"
#include "np_int8.hxx"
#include "np_string.hxx"
#include <string>

namespace NanoPack {

std::shared_ptr<NanoPack::DataType>
create_type_from_identifier(const std::string &identifier);

}

#endif // NANOPACK_NANOC_TYPE_FACTORY_HXX
