#include "person.np.hxx"
#include <iostream>
#include <optional>

int main() {
	std::cout << "A simple program demonstrating conversion between NanoPack "
				 "data and C++ struct."
			  << std::endl;

	Person person;
	person.first_name = "John";
	person.middle_name = std::nullopt;
	person.last_name = "Doe";
	person.age = 40;

	NanoBuf buf = person.data();
	std::cout << "raw bytes:";
	buf.print_bytes();
	std::cout << "size of serialized data in bytes: " << buf.size()
			  << std::endl;

	Person person1(buf.bytes());
	std::cout << "First name: " << person1.first_name << std::endl;
	std::cout << "Last name: " << person1.last_name << std::endl;
	if (!person1.middle_name.has_value()) {
		std::cout << "This person does not have a middle name." << std::endl;
	}
	std::cout << "Age: " << person1.age << std::endl;

	return 0;
}
