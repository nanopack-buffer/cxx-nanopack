#ifndef NANOPACK_NANOC_NP_OPTIONAL_HXX
#define NANOPACK_NANOC_NP_OPTIONAL_HXX

#include "data_type.hxx"

namespace NanoPack {

class Optional : public DataType {
  private:
	std::shared_ptr<DataType> value_type;

  public:
	inline static const std::string IDENTIFIER = "?";

	explicit Optional(std::shared_ptr<DataType> value_type);

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;

	std::shared_ptr<DataType> get_value_type();
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_OPTIONAL_HXX
