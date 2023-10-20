#include "type_factory.hxx"

std::shared_ptr<DataType>
create_type_from_identifier(const std::string &identifier) {
	if (identifier == NanoPackBool::IDENTIFIER) {
		return std::make_shared<NanoPackBool>();
	}
	if (identifier == NanoPackInt8::IDENTIFIER) {
		return std::make_shared<NanoPackInt8>();
	}
	if (identifier == NanoPackInt32::IDENTIFIER) {
		return std::make_shared<NanoPackInt32>();
	}
	if (identifier == NanoPackDouble::IDENTIFIER) {
		return std::make_shared<NanoPackInt32>();
	}
	if (identifier == NanoPackString::IDENTIFIER) {
		return std::make_shared<NanoPackString>();
	}
	// TODO: support message type as type literal
	return nullptr;
}
