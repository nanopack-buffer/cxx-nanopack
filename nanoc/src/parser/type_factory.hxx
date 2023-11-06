#ifndef NANOPACK_NANOC_TYPE_FACTORY_HXX
#define NANOPACK_NANOC_TYPE_FACTORY_HXX

#include "../data_type/data_type.hxx"
#include "../data_type/np_bool.hxx"
#include "../data_type/np_double.hxx"
#include "../data_type/np_int32.hxx"
#include "../data_type/np_int8.hxx"
#include "../data_type/np_string.hxx"
#include <string>

namespace NanoPack {

std::unique_ptr<NanoPack::DataType>
create_type_from_literal(const std::string &literal);

}

#endif // NANOPACK_NANOC_TYPE_FACTORY_HXX
