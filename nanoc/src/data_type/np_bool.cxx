#include "np_bool.hxx"

std::string NanoPack::Bool::identifier() { return IDENTIFIER; }

bool NanoPack::Bool::is_fixed_size() { return true; }

bool NanoPack::Bool::is_user_defined() { return false; }

NanoPack::Size NanoPack::Bool::size() { return 1; }
