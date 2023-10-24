#ifndef NANOPACK_NANOC_CODE_OUTPUT_STREAM_HXX
#define NANOPACK_NANOC_CODE_OUTPUT_STREAM_HXX

#include "iostream"
#include "memory"
#include <unordered_set>

class CodeOutput {
  private:
	std::ostream &ostream;
	std::unordered_set<std::string> variables_in_scope;

  public:
	CodeOutput(std::ostream &ostream);

	std::ostream &stream();

	void add_variable_to_scope(const std::string& var_name);

	void remove_variable_from_scope(const std::string &var_name);

	bool is_variable_in_scope(const std::string &var_name);
};

#endif // NANOPACK_NANOC_CODE_OUTPUT_STREAM_HXX
