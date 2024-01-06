#ifndef NANOPACK_NANOC_CASE_CONV_HXX
#define NANOPACK_NANOC_CASE_CONV_HXX

#include <string>

std::string camel_to_snake(const std::string &camel);

std::string pascal_to_screaming(const std::string &camel);

std::string pascal_to_kebab(const std::string &pascal);

std::string snake_to_camel(const std::string &snake);

std::string snake_to_pascal(const std::string &snake);

std::string snake_to_kebab(const std::string &snake);

#endif // NANOPACK_NANOC_CASE_CONV_HXX
