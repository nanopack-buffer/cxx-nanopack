#ifndef NANOPACK_NANOC_NP_INT64_HXX
#define NANOPACK_NANOC_NP_INT64_HXX

#include "data_type.hxx"

namespace NanoPack {

class Int64 : public DataType {
  public:
	inline static const std::string IDENTIFIER = "int64";

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;
};

} // namespace NanoPack

#endif // NP_INT64_HXX
