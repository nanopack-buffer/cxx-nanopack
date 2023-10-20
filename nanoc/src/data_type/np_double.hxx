#ifndef NANOPACK_NP_DOUBLE_HXX
#define NANOPACK_NP_DOUBLE_HXX

#include "data_type.hxx"

class NanoPackDouble : public DataType {
  public:
	inline static const std::string IDENTIFIER = "double";

	std::string identifier() override;

	bool is_fixed_size() override;
};

#endif // NANOPACK_NP_DOUBLE_HXX
