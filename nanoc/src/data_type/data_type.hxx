#ifndef NANOPACK_NANOC_DATA_TYPE_HXX
#define NANOPACK_NANOC_DATA_TYPE_HXX

#include <iostream>

class DataType {
  public:
	virtual ~DataType() = default;

	virtual std::string identifier() = 0;

	virtual bool is_fixed_size() = 0;
};

#endif // NANOPACK_NANOC_DATA_TYPE_HXX
