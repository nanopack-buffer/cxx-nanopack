#include "np_int32.hxx"

std::string NanoPack::Int32::identifier() { return IDENTIFIER; }

bool NanoPack::Int32::is_fixed_size() { return true; }

bool NanoPack::Int32::is_user_defined() { return false; };

NanoPack::Size NanoPack::Int32::size() { return 4; }
