#ifndef NANOPACK_NANOC_NP_STRING_HXX
#define NANOPACK_NANOC_NP_STRING_HXX

#include "data_type.hxx"

namespace NanoPack {

class String : public DataType {
  public:
	inline static const std::string IDENTIFIER = "string";

	std::string identifier() override;

	bool is_fixed_size() override;

	Size size() override;
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_STRING_HXX
