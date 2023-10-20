#ifndef NANOPACK_NANOC_NP_INT8_HXX
#define NANOPACK_NANOC_NP_INT8_HXX

#include "data_type.hxx"

class NanoPackInt8 : public DataType {
  public:
	inline static const std::string IDENTIFIER = "int8";

	std::string identifier() override;

	bool is_fixed_size() override;
};

#endif // NANOPACK_NANOC_NP_INT8_HXX
