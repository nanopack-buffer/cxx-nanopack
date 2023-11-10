#ifndef NANOPACK_NANOC_NP_MESSAGE_HXX
#define NANOPACK_NANOC_NP_MESSAGE_HXX

#include "data_type.hxx"

namespace NanoPack {

class GenericMessage : public DataType {
  public:
	inline static const std::string IDENTIFIER = "message";

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;
};

class Message : public DataType {
  private:
	std::string message_name;

  public:
	Message(std::string message_name);

	std::string identifier() override;

	bool is_fixed_size() override;

	bool is_user_defined() override;

	Size size() override;
};

} // namespace NanoPack

#endif // NANOPACK_NANOC_NP_MESSAGE_HXX
