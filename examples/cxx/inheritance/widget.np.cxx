#include "widget.np.hxx"
#include <nanopack/reader.hxx>
#include <nanopack/writer.hxx>

Widget::Widget(int32_t id) : id(id) {}

Widget::Widget(const std::vector<uint8_t>::const_iterator begin,
               int &bytes_read) {
  const NanoPack::Reader reader(begin);
  int ptr = 8;

  if (reader.read_type_id() != TYPE_ID) {
    throw "incompatible type";
  }

  const int32_t id = reader.read_int32(ptr);
  ptr += sizeof(int32_t);
  this->id = id;

  bytes_read = ptr;
}

std::vector<uint8_t> Widget::data() const {
  std::vector<uint8_t> buf(sizeof(int32_t) * 2);
  NanoPack::Writer writer(&buf);

  writer.write_type_id(TYPE_ID);

  writer.write_field_size(0, sizeof(int32_t));
  writer.append_int32(id);

  return buf;
}
