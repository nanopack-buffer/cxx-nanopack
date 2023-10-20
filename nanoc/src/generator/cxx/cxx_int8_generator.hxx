#ifndef NANOPACK_NANOC_CXX_INT8_GENERATOR_HXX
#define NANOPACK_NANOC_CXX_INT8_GENERATOR_HXX

#include "../data_type_code_generator.hxx"

class CxxInt8Generator : public DataTypeCodeGenerator {
  public:
	void generate_field_declaration(std::ostream &out,
									const MessageField &field) override;

	void generate_read_code(std::ostream &out,
							const std::string &var_name) override;

	void generate_read_code(std::ostream &out,
							const MessageField &field) override;

	void generate_write_field_code(std::ostream &out,
								   const MessageField &field) override;
};

#endif // NANOPACK_NANOC_CXX_INT8_GENERATOR_HXX
