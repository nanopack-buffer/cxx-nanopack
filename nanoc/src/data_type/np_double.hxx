#ifndef NANOPACK_NP_DOUBLE_HXX
#define NANOPACK_NP_DOUBLE_HXX

#include "data_type.hxx"

namespace NanoPack {

class Double : public DataType {
  public:
	inline static const std::string IDENTIFIER = "double";

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;
};

} // namespace NanoPack

#endif // NANOPACK_NP_DOUBLE_HXX
