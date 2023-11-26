#include "../simple/person.np.hxx"
#include <iostream>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

int main() {
	auto friend_ =
		std::make_shared<Person>("Steve", std::nullopt, "Jobs", 52, nullptr);
	Person person("John", std::nullopt, "Doe", 40, std::move(friend_));
	const char *person_json_str =
		"{\"firstName\":\"John\",\"middleName\":null,\"lastName\":\"Doe\","
		"\"age\":40,\"otherFriend\":{\"firstName\":\"Steve\",\"middleName\":"
		"null,\"lastName\":\"Jobs\",\"age\":52,\"otherFriend\":null}}";
	const std::vector<uint8_t> nanobuf_data = person.data();

	constexpr int SAMPLE_SIZE = 10000000;

	const auto a = std::chrono::duration_cast<std::chrono::milliseconds>(
					   std::chrono::system_clock::now().time_since_epoch())
					   .count();
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		rapidjson::Document d;
		d.Parse(person_json_str);

		rapidjson::StringBuffer buf;
		rapidjson::Writer writer(buf);
		d.Accept(writer);

		const std::string json = buf.GetString();
	}
	const auto b = std::chrono::duration_cast<std::chrono::milliseconds>(
					   std::chrono::system_clock::now().time_since_epoch())
					   .count();
	std::cout << b - a << std::endl;

	const auto c = std::chrono::duration_cast<std::chrono::milliseconds>(
					   std::chrono::system_clock::now().time_since_epoch())
					   .count();
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		int bytes_read = 0;
		Person p(nanobuf_data.begin(), bytes_read);
		const std::vector<uint8_t> data = p.data();
	}
	const auto d = std::chrono::duration_cast<std::chrono::milliseconds>(
					   std::chrono::system_clock::now().time_since_epoch())
					   .count();
	std::cout << d - c << std::endl;

	return 0;
}
