#ifndef NANOPACK_NANOC_NP_INT32_HXX
#define NANOPACK_NANOC_NP_INT32_HXX

#include "data_type.hxx"

namespace NanoPack {

class Int32 : public DataType {
  public:
	inline static const std::string IDENTIFIER = "int32";

	std::string identifier() override;

	bool is_fixed_size() override;

	Size size() override;
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_INT32_HXX
