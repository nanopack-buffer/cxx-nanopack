#include "person.np.hxx"
#include <nanopack/reader.hxx>
#include <nanopack/writer.hxx>

Person::Person(std::string first_name, std::optional<std::string> middle_name,
               std::string last_name, int32_t age,
               std::shared_ptr<Person> other_friend)
    : first_name(std::move(first_name)), middle_name(middle_name),
      last_name(std::move(last_name)), age(age),
      other_friend(std::move(other_friend)) {}

Person::Person(const std::vector<uint8_t>::const_iterator begin,
               int &bytes_read) {
  const NanoPack::Reader reader(begin);
  int ptr = 24;

  if (reader.read_type_id() != TYPE_ID) {
    throw "incompatible type";
  }

  const int32_t first_name_size = reader.read_field_size(0);
  first_name = reader.read_string(ptr, first_name_size);
  ptr += first_name_size;

  if (reader.read_field_size(1) < 0) {
    this->middle_name = std::nullopt;
  } else {
    const int32_t middle_name_size = reader.read_field_size(1);
    middle_name = reader.read_string(ptr, middle_name_size);
    ptr += middle_name_size;
  }

  const int32_t last_name_size = reader.read_field_size(2);
  last_name = reader.read_string(ptr, last_name_size);
  ptr += last_name_size;

  const int32_t age = reader.read_int32(ptr);
  ptr += sizeof(int32_t);
  this->age = age;

  if (reader.read_field_size(4) < 0) {
    other_friend = nullptr;
  } else {
    int other_friend_bytes_read = 0;
    other_friend =
        std::make_shared<Person>(begin + ptr, other_friend_bytes_read);
    ptr += other_friend_bytes_read;
  }

  bytes_read = ptr;
}

std::vector<uint8_t> Person::data() const {
  std::vector<uint8_t> buf(sizeof(int32_t) * 6);
  NanoPack::Writer writer(&buf);

  writer.write_type_id(TYPE_ID);

  writer.write_field_size(0, first_name.size());
  writer.append_string(first_name);

  if (middle_name.has_value()) {
    auto middle_name = this->middle_name.value();
    writer.write_field_size(1, middle_name.size());
    writer.append_string(middle_name);
  } else {
    writer.write_field_size(1, -1);
  }

  writer.write_field_size(2, last_name.size());
  writer.append_string(last_name);

  writer.write_field_size(3, sizeof(int32_t));
  writer.append_int32(age);

  if (other_friend != nullptr) {
    const std::vector<uint8_t> other_friend_data = other_friend->data();
    writer.append_bytes(other_friend_data);
    writer.write_field_size(4, other_friend_data.size());
  } else {
    writer.write_field_size(4, -1);
  }

  return buf;
}
