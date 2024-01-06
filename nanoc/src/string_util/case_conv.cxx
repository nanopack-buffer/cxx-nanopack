#include "case_conv.hxx"
#include <sstream>

std::string camel_to_snake(const std::string &camel) {
	std::stringstream snake;
	snake << (char)tolower(camel[0]);

	const size_t len = camel.length();
	for (int i = 1; i < len; i++) {
		const char ch = camel[i];
		if (isupper(ch)) {
			snake << "_" << (char)tolower(ch);
		} else {
			snake << ch;
		}
	}

	return snake.str();
}

std::string pascal_to_screaming(const std::string &camel) {
	std::stringstream ss;
	ss << camel[0];

	const size_t len = camel.length();
	for (int i = 1; i < len; i++) {
		const char ch = camel[i];
		if (isupper(ch)) {
			ss << "_";
		}
		ss << (char)toupper(ch);
	}

	return ss.str();
}

std::string pascal_to_kebab(const std::string &pascal) {
	std::stringstream ss;
	ss << static_cast<char>(tolower(pascal[0]));

	const size_t len = pascal.length();
	for (int i = 1; i < len; i++) {
		const char ch = pascal[i];
		if (isupper(ch)) {
			ss << "-" << static_cast<char>(tolower(ch));
		} else {
			ss << ch;
		}
	}

	return ss.str();
}

std::string snake_to_camel(const std::string &snake) {
	std::stringstream camel;

	size_t i = 0;
	const size_t strlen = snake.length();
	while (i < strlen) {
		const char &ch = snake[i];
		if (ch == '_') {
			camel << (char)toupper(snake[++i]);
		} else {
			camel << ch;
		}
		i++;
	}

	return camel.str();
}

std::string snake_to_pascal(const std::string &snake) {
	std::stringstream camel;
	camel << (char)toupper(snake[0]);

	size_t i = 1;
	const size_t strlen = snake.length();
	while (i < strlen) {
		const char &ch = snake[i];
		if (ch == '_') {
			camel << (char)toupper(snake[++i]);
		} else {
			camel << ch;
		}
		i++;
	}

	return camel.str();
}

std::string snake_to_kebab(const std::string &snake) {
	std::stringstream kebab;
	for (const char &ch : snake) {
		if (ch == '_') {
			kebab << '-';
		} else {
			kebab << ch;
		}
	}
	return kebab.str();
}
