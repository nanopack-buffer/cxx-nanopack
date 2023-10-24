#include "np_bool.hxx"

std::string NanoPack::Bool::identifier() { return IDENTIFIER; }

bool NanoPack::Bool::is_fixed_size() { return true; }

NanoPack::Size NanoPack::Bool::size() { return 1; }
