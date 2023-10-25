#ifndef NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX
#define NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX

#include "../message_schema.hxx"
#include "code_output.hxx"
#include <iostream>
#include <map>

class DataTypeCodeGenerator {
  public:
	virtual ~DataTypeCodeGenerator() = default;

	virtual std::string get_type_declaration(NanoPack::DataType *data_type) = 0;

	virtual std::string
	get_read_size_expression(NanoPack::DataType *data_type,
							 const std::string &var_name) = 0;

	virtual void generate_field_declaration(CodeOutput &output,
											const MessageField &field) = 0;

	/**
	 * Generates code for reading value of this data type into a variable from a
	 * NanoPack buffer.
	 *
	 * @param output The output stream to write the code to.
	 * @param var_name The variable name that stores the read value.
	 */
	virtual void generate_read_code(CodeOutput &output,
									NanoPack::DataType *type,
									const std::string &var_name) = 0;

	virtual void generate_read_code(CodeOutput &output,
									const MessageField &field) = 0;

	virtual void generate_write_code(CodeOutput &output,
									 NanoPack::DataType *type,
									 const std::string &var_name) = 0;

	/**
	 * Generates code for writing the value of a variable into a NanoPack
	 * buffer.
	 *
	 * @param output The output stream to write the code to.
	 * @param field_name The name of the variable that stores the value to be
	 * written to a NanoPack buffer.
	 */
	virtual void generate_write_code(CodeOutput &output,
									 const MessageField &field) = 0;
};

class DataTypeCodeGeneratorRegistry {
  private:
	std::map<std::string, std::shared_ptr<DataTypeCodeGenerator>> generators;

  public:
	DataTypeCodeGeneratorRegistry();

	void
	add_generator_for_type(NanoPack::DataType *data_type,
						   std::shared_ptr<DataTypeCodeGenerator> generator);

	void
	add_generator_for_type(const std::string &type_identifier,
						   std::shared_ptr<DataTypeCodeGenerator> generator);

	std::shared_ptr<DataTypeCodeGenerator>
	find_generator_for_type(NanoPack::DataType *data_type);

	std::shared_ptr<DataTypeCodeGenerator>
	find_generator_for_type(std::string type_identifier);
};

#endif // NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX
