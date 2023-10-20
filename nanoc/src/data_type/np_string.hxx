#ifndef NANOPACK_NANOC_NP_STRING_HXX
#define NANOPACK_NANOC_NP_STRING_HXX

#include "data_type.hxx"

class NanoPackString : public DataType {
  public:
	inline static const std::string IDENTIFIER = "string";

	std::string identifier() override;

	bool is_fixed_size() override;
};

#endif // NANOPACK_NANOC_NP_STRING_HXX
