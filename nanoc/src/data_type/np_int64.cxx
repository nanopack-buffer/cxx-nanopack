#include "np_int64.hxx"

std::string NanoPack::Int64::identifier() { return IDENTIFIER; }

bool NanoPack::Int64::is_fixed_size() { return true; }

bool NanoPack::Int64::is_user_defined() { return false; };

NanoPack::Size NanoPack::Int64::size() { return 8; }
