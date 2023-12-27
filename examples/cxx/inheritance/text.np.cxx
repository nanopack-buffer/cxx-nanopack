#include "text.np.hxx"
#include <nanopack/reader.hxx>
#include <nanopack/writer.hxx>

Text::Text(int32_t id, std::string content)
    : Widget(id), content(std::move(content)) {}

Text::Text(const NanoPack::Reader &reader, int &bytes_read) : Widget() {
  const auto begin = reader.begin();
  int ptr = 12;

  const int32_t id = reader.read_int32(ptr);
  ptr += sizeof(int32_t);
  this->id = id;

  const int32_t content_size = reader.read_field_size(1);
  content = reader.read_string(ptr, content_size);
  ptr += content_size;

  bytes_read = ptr;
}

Text::Text(std::vector<uint8_t>::const_iterator begin, int &bytes_read)
    : Text(NanoPack::Reader(begin), bytes_read) {}

std::vector<uint8_t> Text::data() const {
  std::vector<uint8_t> buf(sizeof(int32_t) * 3);
  NanoPack::Writer writer(&buf);

  writer.write_type_id(TYPE_ID);

  writer.write_field_size(0, sizeof(int32_t));
  writer.append_int32(id);

  writer.write_field_size(1, content.size());
  writer.append_string(content);

  return buf;
}
