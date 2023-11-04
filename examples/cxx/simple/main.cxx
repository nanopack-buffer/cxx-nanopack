#include "person.np.hxx"
#include <iostream>

int main() {
	std::cout << "simple program demonstrating converting between nanopack "
				 "data and c++ struct."
			  << std::endl;

	Person person;
	person.first_name = "John";
	person.last_name = "Doe";
	person.age = 40;

	NanoBuf buf = person.data();
	std::cout << "size of serialized data in bytes: " << buf.size()
			  << std::endl;

	Person person1(buf.bytes());
	std::cout << "First name: " << person1.first_name << std::endl;
	std::cout << "Last name: " << person1.last_name << std::endl;
	std::cout << "Age: " << person1.age << std::endl;

	return 0;
}
