#include "np_int8.hxx"

std::string NanoPack::Int8::identifier() { return IDENTIFIER; }

bool NanoPack::Int8::is_fixed_size() { return true; }

bool NanoPack::Int8::is_user_defined() { return false; }

NanoPack::Size NanoPack::Int8::size() { return 1; }
