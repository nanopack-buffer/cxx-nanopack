#ifndef NANOPACK_NANOC_NP_ARRAY_HXX
#define NANOPACK_NANOC_NP_ARRAY_HXX

#include "data_type.hxx"
#include <memory>

namespace NanoPack {

class Array : public DataType {
  private:
	std::shared_ptr<DataType> item_type;

  public:
	inline static const std::string ARRAY_IDENTIFIER_START = "[";
	inline static const std::string ARRAY_IDENTIFIER_END = "]";
	inline static const std::string IDENTIFIER = "[]";

	explicit Array(std::shared_ptr<DataType> item_type);

	std::string identifier() override;

	bool is_fixed_size() override;

	Size size() override;

	std::shared_ptr<DataType> get_item_type();
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_ARRAY_HXX
