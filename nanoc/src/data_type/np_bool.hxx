#ifndef NANOPACK_NANOC_NP_BOOL_HXX
#define NANOPACK_NANOC_NP_BOOL_HXX

#include "data_type.hxx"

namespace NanoPack {

class Bool : public DataType {
  public:
	inline static const std::string IDENTIFIER = "bool";

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_BOOL_HXX
