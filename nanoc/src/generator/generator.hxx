#ifndef NANOPACK_NANOC_GENERATOR_HXX
#define NANOPACK_NANOC_GENERATOR_HXX

#include "../message_schema.hxx"
#include "data_type_code_generator.hxx"

class Generator {
  public:
	virtual ~Generator() = default;

	virtual void generate_for_schema(const MessageSchema &schema) = 0;
};

#endif // NANOPACK_NANOC_GENERATOR_HXX
