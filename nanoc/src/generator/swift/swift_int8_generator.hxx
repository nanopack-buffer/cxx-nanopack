#ifndef NANOPACK_NANOC_SWIFT_INT8_GENERATOR_HXX
#define NANOPACK_NANOC_SWIFT_INT8_GENERATOR_HXX

#include "../data_type_code_generator.hxx"

class SwiftInt8Generator : public DataTypeCodeGenerator {
  public:
	std::string get_type_declaration(NanoPack::DataType *data_type) override;

	std::string get_read_size_expression(NanoPack::DataType *data_type,
										 const std::string &var_name) override;

	void generate_field_declaration(CodeOutput &output,
									const MessageField &field) override;

	void generate_read_code(CodeOutput &output, NanoPack::DataType *type,
							const std::string &var_name) override;

	void generate_read_code(CodeOutput &output,
							const MessageField &field) override;

	void generate_write_code(CodeOutput &output, NanoPack::DataType *type,
							 const std::string &var_name) override;

	void generate_write_code(CodeOutput &output,
							 const MessageField &field) override;
};

#endif // NANOPACK_NANOC_SWIFT_INT8_GENERATOR_HXX
