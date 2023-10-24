#ifndef NANOPACK_NANOC_DATA_TYPE_HXX
#define NANOPACK_NANOC_DATA_TYPE_HXX

#include <iostream>

namespace NanoPack {

using Size = int32_t;

class DataType {
  public:
	const static Size DYNAMIC_SIZE = -1;

	virtual ~DataType() = default;

	virtual std::string identifier() = 0;

	virtual bool is_fixed_size() = 0;

	virtual Size size() = 0;
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_DATA_TYPE_HXX
