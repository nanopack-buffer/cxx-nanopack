#include "case_conv.hxx"

std::string camel_to_snake(std::string camel) {
	std::string snake;
	snake += tolower(camel[0]);

	const int len = camel.length();
	for (int i = 1; i < len; i++) {
		const char ch = camel[i];
		if (isupper(ch)) {
			snake += "_";
			snake += tolower(ch);
		} else {
			snake += ch;
		}
	}

	return snake;
}
