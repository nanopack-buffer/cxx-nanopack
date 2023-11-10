#include <utility>

#include "np_message.hxx"

std::string NanoPack::GenericMessage::identifier() {
	return NanoPack::GenericMessage::IDENTIFIER;
}

bool NanoPack::GenericMessage::is_fixed_size() { return false; }

bool NanoPack::GenericMessage::is_user_defined() { return false; }

NanoPack::Size NanoPack::GenericMessage::size() {
	return NanoPack::DataType::DYNAMIC_SIZE;
}

NanoPack::Message::Message(std::string message_name)
	: message_name(std::move(message_name)) {}

std::string NanoPack::Message::identifier() { return message_name; }

bool NanoPack::Message::is_fixed_size() { return false; }

bool NanoPack::Message::is_user_defined() { return true; }

NanoPack::Size NanoPack::Message::size() {
	return NanoPack::DataType::DYNAMIC_SIZE;
}
