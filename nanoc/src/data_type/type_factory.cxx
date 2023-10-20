#include "type_factory.hxx"

DataType *create_type_from_identifier(const std::string &identifier) {
	if (identifier == NanoPackBool::IDENTIFIER) {
		return new NanoPackBool();
	}
	if (identifier == NanoPackInt8::IDENTIFIER) {
		return new NanoPackInt8();
	}
	if (identifier == NanoPackInt32::IDENTIFIER) {
		return new NanoPackInt32();
	}
	if (identifier == NanoPackDouble::IDENTIFIER) {
		return new NanoPackDouble();
	}
	if (identifier == NanoPackString::IDENTIFIER) {
		return new NanoPackString();
	}
	// TODO: support message type as type literal
	return nullptr;
}
