#ifndef NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX
#define NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX

#include "../message_schema.hxx"
#include "code_output.hxx"
#include <iostream>
#include <map>

/**
 * Describes a code generator for a specific NanoPack data type, e.g. string,
 * int32. Each language should implement this interface for each NanoPack data
 * type.
 */
class DataTypeCodeGenerator {
  public:
	virtual ~DataTypeCodeGenerator() = default;

	virtual std::string get_type_declaration(NanoPack::DataType *data_type) = 0;

	/**
	 * Constructs an expression for calculating the byte size of the given
	 * variable and of the given data type.
	 *
	 * @param data_type Additional type info for the variable.
	 * @param var_name The name of the variable whose size should be read from.
	 * @return An expression that evaluates to the byte size of the variable.
	 */
	virtual std::string
	get_read_size_expression(NanoPack::DataType *data_type,
							 const std::string &var_name) = 0;

	virtual void generate_constructor_parameter(CodeOutput &output,
												const MessageField &field) = 0;

	virtual void
	generate_constructor_field_initializer(CodeOutput &output,
										   const MessageField &field) = 0;

	/**
	 * Generates language-specific declaration (e.g. a C++ class field.) for the
	 * given NanoPack message field,
	 *
	 * @param output
	 * @param field
	 */
	virtual void generate_field_declaration(CodeOutput &output,
											const MessageField &field) = 0;

	/**
	 * Generates code for reading value of this data type into a variable from a
	 * NanoPack buffer.
	 *
	 * @param output The output stream to write the code to.
	 * @param type Additional type info for the variable. It is safe to assume
	 * that this type is always passed to the correct type of generator.
	 * @param var_name The variable name that stores the read value.
	 */
	virtual void generate_read_code(CodeOutput &output,
									NanoPack::DataType *type,
									const std::string &var_name) = 0;

	/**
	 * Generates code for reading value of this data type into a NanoPack
	 * message field from a NanoPack buffer.
	 *
	 * @param output The output stream to write the code to.
	 * @param field The field that stores the read value.
	 */
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

/**
 * Holds instances of DataTypeCodeGenerator that can be looked up
 * by either the correct type identifier or an instance of NanoPack::DataType
 *
 * @see DataTypeCodeGenerator
 */
class DataTypeCodeGeneratorRegistry {
  private:
	std::map<std::string, std::shared_ptr<DataTypeCodeGenerator>> generators;

  public:
	DataTypeCodeGeneratorRegistry();

	/**
	 * Registers the given generator with the given data type.
	 *
	 * @param data_type The data type that the generator is responsible for
	 * generating code.
	 * @param generator The DataTypeCodeGenerator that is responsible for
	 * generating code of the given data type.
	 */
	void
	add_generator_for_type(NanoPack::DataType *data_type,
						   std::shared_ptr<DataTypeCodeGenerator> generator);

	/**
	 * Registers the given generator with the given data type.
	 *
	 * @param type_identifier The identifier of the data type that the generator
	 * is responsible for generating code.
	 * @param generator The DataTypeCodeGenerator that is responsible for
	 * generating code of the given data type.
	 */
	void
	add_generator_for_type(const std::string &type_identifier,
						   std::shared_ptr<DataTypeCodeGenerator> generator);

	/**
	 * Finds the generator that is responsible for generating code of the given
	 * type.
	 *
	 * @return A shared pointer to the instance of DataTypeCodeGenerator that
	 * can generate code for the data type, or nullptr if none is found.
	 */
	std::shared_ptr<DataTypeCodeGenerator>
	find_generator_for_type(NanoPack::DataType *data_type);

	/**
	 * Finds the generator that is responsible for generating code of the given
	 * type.
	 *
	 * @param type_identifier The identifier of the data type interested.
	 * @return A shared pointer to the instance of DataTypeCodeGenerator that
	 * can generate code for the data type, or nullptr if none is found.
	 */
	std::shared_ptr<DataTypeCodeGenerator>
	find_generator_for_type(std::string type_identifier);
};

#endif // NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX
