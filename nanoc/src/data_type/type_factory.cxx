#include "type_factory.hxx"
#include "np_array.hxx"
#include "np_int32.hxx"
#include "np_int8.hxx"
#include "np_string.hxx"

std::shared_ptr<NanoPack::DataType>
NanoPack::create_type_from_identifier(const std::string &identifier) {
	if (identifier == NanoPack::Bool::IDENTIFIER) {
		return std::make_shared<NanoPack::Bool>();
	}
	if (identifier == NanoPack::Int8::IDENTIFIER) {
		return std::make_shared<NanoPack::Int8>();
	}
	if (identifier == NanoPack::Int32::IDENTIFIER) {
		return std::make_shared<NanoPack::Int32>();
	}
	if (identifier == NanoPack::Double::IDENTIFIER) {
		return std::make_shared<NanoPack::Double>();
	}
	if (identifier == NanoPack::String::IDENTIFIER) {
		return std::make_shared<NanoPack::String>();
	}

	// TODO: perhaps set a limit on type nesting level?
	const auto pos = identifier.rfind(NanoPack::Array::IDENTIFIER);
	if (pos != std::string::npos) {
		const auto array_type_identifier = identifier.substr(0, pos);
		const auto array_type =
			create_type_from_identifier(array_type_identifier);
		if (array_type == nullptr) {
			return nullptr;
		}
		return std::make_shared<NanoPack::Array>(array_type);
	}

	// TODO: support message type as type literal
	return nullptr;
}
