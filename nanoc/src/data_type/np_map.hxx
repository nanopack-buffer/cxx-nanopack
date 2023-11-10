#ifndef NANOPACK_NANOC_NP_MAP_HXX
#define NANOPACK_NANOC_NP_MAP_HXX

#include "data_type.hxx"

namespace NanoPack {

class Map : public DataType {
  private:
	std::shared_ptr<DataType> key_type;
	std::shared_ptr<DataType> value_type;

  public:
	inline static const std::string MAP_IDENTIFIER_START = "<";
	inline static const std::string MAP_IDENTIFIER_END = ">";
	inline static const std::string IDENTIFIER = "<>";

	Map(std::shared_ptr<DataType> key_type,
		std::shared_ptr<DataType> value_type);

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;

	std::shared_ptr<DataType> get_key_type();

	std::shared_ptr<DataType> get_value_type();
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_MAP_HXX
