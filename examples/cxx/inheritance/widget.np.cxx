#include "widget.np.hxx"
#include <nanopack/reader.hxx>
#include <nanopack/writer.hxx>

#include "text.np.hxx"

std::unique_ptr<Widget> Widget::from(std::vector<uint8_t>::const_iterator begin,
                                     int &bytes_read) {
  const NanoPack::Reader reader(begin);
  const auto type_id = reader.read_type_id();
  switch (type_id) {
  case TYPE_ID:
    return std::make_unique<Widget>(reader, bytes_read);
  case Text::TYPE_ID:
    return std::make_unique<Text>(reader, bytes_read);
  default:
    return nullptr;
  }
}

Widget::Widget(int32_t id) : id(id) {}

Widget::Widget(const NanoPack::Reader &reader, int &bytes_read) {
  int ptr = 8;

  if (reader.read_type_id() != TYPE_ID) {
    throw "incompatible type";
  }

  const int32_t id = reader.read_int32(ptr);
  ptr += sizeof(int32_t);
  this->id = id;

  bytes_read = ptr;
}

Widget::Widget(std::vector<uint8_t>::const_iterator begin, int &bytes_read)
    : Widget(NanoPack::Reader(begin), bytes_read) {}

std::vector<uint8_t> Widget::data() const {
  std::vector<uint8_t> buf(sizeof(int32_t) * 2);
  NanoPack::Writer writer(&buf);

  writer.write_type_id(TYPE_ID);

  writer.write_field_size(0, sizeof(int32_t));
  writer.append_int32(id);

  return buf;
}
