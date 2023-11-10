#include <utility>

#include "code_output.hxx"

CodeOutput::CodeOutput(std::ostream &ostream,
					   const MessageSchema &message_schema)
	: ostream(ostream), message_schema(message_schema), variables_in_scope() {}

std::ostream &CodeOutput::stream() { return ostream; }

const MessageSchema &CodeOutput::get_message_schema() { return message_schema; }

void CodeOutput::add_variable_to_scope(const std::string &var_name) {
	variables_in_scope.insert(var_name);
}

void CodeOutput::remove_variable_from_scope(const std::string &var_name) {
	variables_in_scope.erase(var_name);
}

bool CodeOutput::is_variable_in_scope(const std::string &var_name) {
	return variables_in_scope.find(var_name) != variables_in_scope.end();
}
