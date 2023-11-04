#include "person.np.hxx"

Person::Person() {}

Person::Person(std::vector<uint8_t> &data) {
  NanoBuf buf(data);
  int ptr = 24;

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

  const int32_t numbers_size = buf.read_field_size(3);
  const int32_t numbers_vec_vec_size = numbers_size / 4;
  std::vector<int32_t> numbers_vec(numbers_vec_vec_size);
  for (int i = 0; i < numbers_vec_vec_size; i++) {
    const int32_t i_item = buf.read_int32(ptr);
    ptr += sizeof(int32_t);

    numbers_vec[i] = i_item;
  }
  this->numbers = numbers_vec;

  const int32_t maps_vec_vec_size = buf.read_int32(ptr);
  ptr += sizeof(int32_t);
  std::vector<std::unordered_map<int32_t, std::string>> maps_vec(
      maps_vec_vec_size);
  for (int i = 0; i < maps_vec_vec_size; i++) {
    const int32_t i_item_map_size = buf.read_int32(ptr);
    ptr += sizeof(int32_t);
    std::unordered_map<int32_t, std::string> i_item;
    i_item.reserve(i_item_map_size);
    for (int ii = 0; ii < i_item_map_size; ii++) {
      const int32_t ii_key = buf.read_int32(ptr);
      ptr += sizeof(int32_t);
      const int32_t ii_value_size = buf.read_int32(ptr);
      ptr += sizeof(int32_t);
      std::string ii_value = buf.read_string(ptr, ii_value_size);
      ptr += ii_value_size;
      i_item.insert({ii_key, ii_value});
    }

    maps_vec[i] = i_item;
  }
  this->maps = maps_vec;
}

NanoBuf Person::data() {
  NanoBuf buf(sizeof(int32_t) * 6);

  buf.write_type_id(Person::TYPE_ID);

  buf.write_field_size(0, first_name.size());
  buf.append_string(first_name);

  buf.write_field_size(1, last_name.size());
  buf.append_string(last_name);

  buf.write_field_size(2, sizeof(int32_t));
  buf.append_int32(age);

  buf.write_field_size(3, 4 * numbers.size());
  for (const auto &item0 : numbers) {
    buf.append_int32(item0);
  }

  int32_t maps_total_size = sizeof(int32_t);
  buf.append_int32(maps.size());
  for (const auto &item0 : maps) {
    buf.append_int32(item0.size());
    int32_t item0_total_size = sizeof(int32_t) + item0.size() * 4;
    for (const auto &item1 : item0) {
      auto item1_key = item1.first;
      auto item1_value = item1.second;
      buf.append_int32(item1_key);
      buf.append_int32(item1_value.size());
      buf.append_string(item1_value);
      item0_total_size += item1_value.size() + sizeof(int32_t);
    }

    maps_total_size += item0_total_size + sizeof(int32_t);
  }

  buf.write_field_size(4, maps_total_size);

  return buf;
}
