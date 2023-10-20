#ifndef NANOPACK_NANOC_NP_BOOL_HXX
#define NANOPACK_NANOC_NP_BOOL_HXX

#include "data_type.hxx"

class NanoPackBool : public DataType {
  public:
	inline static const std::string IDENTIFIER = "bool";

	std::string identifier() override;

	bool is_fixed_size() override;
};

#endif // NANOPACK_NANOC_NP_BOOL_HXX
