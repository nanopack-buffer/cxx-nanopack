#include "np_string.hxx"

std::string NanoPack::String::identifier() { return IDENTIFIER; }

bool NanoPack::String::is_fixed_size() { return false; }

bool NanoPack::String::is_user_defined() { return false; }

NanoPack::Size NanoPack::String::size() { return DYNAMIC_SIZE; }
