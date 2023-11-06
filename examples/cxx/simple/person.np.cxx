#include "person.np.hxx"

Person::Person() {}

Person::Person(std::vector<uint8_t> &data) {
  NanoBuf buf(data);
  int ptr = 20;

  const int32_t type_id = buf.read_type_id();
  if (type_id != Person::TYPE_ID) {
    throw "incompatible type";
  }

  const int32_t first_name_size = buf.read_field_size(0);
  std::string first_name = buf.read_string(ptr, first_name_size);
  ptr += first_name_size;
  this->first_name = first_name;

  if (buf.read_field_size(1) < 0) {
    this->middle_name = std::nullopt;
  } else {
    const int32_t middle_name_size = buf.read_field_size(1);
    std::string middle_name = buf.read_string(ptr, middle_name_size);
    ptr += middle_name_size;
    this->middle_name = middle_name;
  }

  const int32_t last_name_size = buf.read_field_size(2);
  std::string last_name = buf.read_string(ptr, last_name_size);
  ptr += last_name_size;
  this->last_name = last_name;

  const int32_t age = buf.read_int32(ptr);
  ptr += sizeof(int32_t);
  this->age = age;
}

NanoBuf Person::data() {
  NanoBuf buf(sizeof(int32_t) * 5);

  buf.write_type_id(Person::TYPE_ID);

  buf.write_field_size(0, first_name.size());
  buf.append_string(first_name);

  if (middle_name.has_value()) {
    auto middle_name = this->middle_name.value();
    buf.write_field_size(1, middle_name.size());
    buf.append_string(middle_name);
  } else {
    buf.write_field_size(1, -1);
  }

  buf.write_field_size(2, last_name.size());
  buf.append_string(last_name);

  buf.write_field_size(3, sizeof(int32_t));
  buf.append_int32(age);

  return buf;
}
