#include "np_double.hxx"

std::string NanoPack::Double::identifier() { return IDENTIFIER; }

bool NanoPack::Double::is_fixed_size() { return true; }

NanoPack::Size NanoPack::Double::size() { return 8; }
