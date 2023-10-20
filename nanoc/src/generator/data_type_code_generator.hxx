#ifndef NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX
#define NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX

#include "../message_schema.hxx"
#include <iostream>

class DataTypeCodeGenerator {
  public:
	virtual ~DataTypeCodeGenerator() = default;

	virtual void generate_field_declaration(std::ostream &out,
											const MessageField &field) = 0;

	/**
	 * Generates code for reading value of this data type into a variable from a
	 * NanoPack buffer.
	 *
	 * @param out The output stream to write the code to.
	 * @param var_name The variable name that stores the read value.
	 */
	virtual void generate_read_code(std::ostream &out,
									const std::string &var_name) = 0;

	virtual void generate_read_code(std::ostream &out,
									const MessageField &field) = 0;

	/**
	 * Generates code for writing the value of a variable into a NanoPack
	 * buffer.
	 *
	 * @param out The output stream to write the code to.
	 * @param field_name The name of the variable that stores the value to be
	 * written to a NanoPack buffer.
	 */
	virtual void generate_write_field_code(std::ostream &out,
										   const MessageField &field) = 0;
};

#endif // NANOPACK_NANOC_DATA_TYPE_CODE_GENERATOR_HXX
