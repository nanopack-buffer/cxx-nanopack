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

  const int32_t last_name_size = buf.read_field_size(1);
  std::string last_name = buf.read_string(ptr, last_name_size);
  ptr += last_name_size;
  this->last_name = last_name;

  const int32_t age = buf.read_int32(ptr);
  ptr += sizeof(int32_t);
  this->age = age;

  const int32_t numbers_vec_vec_size = buf.read_int32(ptr);
  ptr += sizeof(int32_t);
  std::vector<int32_t> numbers_vec(numbers_vec_vec_size);
  for (int i = 0; i < numbers_vec_vec_size; i++) {
    const int32_t i_item = buf.read_int32(ptr);
    ptr += sizeof(int32_t);

    numbers_vec[i] = i_item;
  }
  this->numbers = numbers_vec;
}

NanoBuf Person::data() {
  NanoBuf buf(sizeof(int32_t) * 5);

  buf.write_type_id(Person::TYPE_ID);

  buf.write_field_size(0, first_name.size());
  buf.append_string(first_name);

  buf.write_field_size(1, last_name.size());
  buf.append_string(last_name);

  buf.write_field_size(2, sizeof(int32_t));
  buf.append_int32(age);

  buf.write_field_size(3, 4 * numbers.size() + sizeof(int32_t));
  buf.append_int32(numbers.size());
  for (auto item0 : numbers) {
    buf.append_int32(item0);
  }

  return buf;
}
