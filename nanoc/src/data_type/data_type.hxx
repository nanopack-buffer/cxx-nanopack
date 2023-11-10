#ifndef NANOPACK_NANOC_DATA_TYPE_HXX
#define NANOPACK_NANOC_DATA_TYPE_HXX

#include <iostream>

namespace NanoPack {

using Size = int32_t;

/**
 * Descriptor of a NanoPack data type.
 */
class DataType {
  public:
	const static Size DYNAMIC_SIZE = -1;

	virtual ~DataType() = default;

	/**
	 * @return String identifier of this data type.
	 */
	virtual std::string identifier() = 0;

	/**
	 * @return Whether this data type has a fixed size.
	 */
	virtual bool is_fixed_size() = 0;

	virtual bool is_user_defined() = 0;

	/**
	 * @return The size in bytes this data type takes in the buffer, or DataType::DYNAMIC_SIZE if this data type has a dynamic size.
	 */
	virtual Size size() = 0;
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_DATA_TYPE_HXX
