#include "message_schema.hxx"

MessageField::~MessageField() { delete type; }
