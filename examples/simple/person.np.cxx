#include "person.np.hxx"

Person::Person() {}

Person::Person(std::vector<uint8_t> &data) {
  NanoBuf buf(data);
  int ptr = 16;

  const int32_t type_id = buf.read_type_id();
  if (type_id != Person::TYPE_ID) {
    throw "incompatible type";
  }

  const int32_t first_name_size = buf.read_field_size(0);
  std::string first_name = buf.read_string(ptr, first_name_size);
  ptr += first_name_size;
  this->first_name = first_name;

  const int32_t last_name_size = buf.read_field_size(1);
  std::string last_name = buf.read_string(ptr, last_name_size);
  ptr += last_name_size;
  this->last_name = last_name;
}

NanoBuf Person::data() {
  NanoBuf buf(sizeof(int32_t) * 4);

  buf.write_type_id(Person::TYPE_ID);

  buf.write_field_size(0, first_name.size());
  buf.append_string(first_name);

  buf.write_field_size(1, last_name.size());
  buf.append_string(last_name);

  return buf;
}
