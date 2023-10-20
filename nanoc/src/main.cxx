#include "generator/cxx/cxx_generator.hxx"
#include "message_schema.hxx"
#include "parser/parse_schema.hxx"
#include <argparse/argparse.hpp>
#include <iostream>
#include <oneapi/tbb/concurrent_vector.h>
#include <optional>
#include <string>
#include <thread>
#include <vector>

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("nanopack");

	program.add_argument("input_files")
		.nargs(argparse::nargs_pattern::at_least_one);
	program.add_argument("-l", "--language")
		.required()
		.help("Specify the language of the generated code.");

	program.parse_args(argc, argv);

	const auto language = program.get<std::string>("--language");
	const auto input_files =
		program.get<std::vector<std::string>>("input_files");

	if (input_files.empty()) {
		std::cerr << "No input file specified!" << std::endl;
		return -1;
	}

	oneapi::tbb::concurrent_vector<MessageSchema> all_schemas;
	std::vector<std::thread> threads;
	for (const std::string &input_file_path : input_files) {
		threads.emplace_back([&all_schemas, input_file_path]() {
			std::optional<MessageSchema> schema =
				parse_schema_file(input_file_path);
			if (schema.has_value()) {
				all_schemas.emplace_back(schema.value());
			}
		});
	}

	for (auto &t : threads) {
		t.join();
	}

	// TODO: should select the appropriate generator based on the language flag
	CxxGenerator generator;
	for (const MessageSchema &schema : all_schemas) {
		generator.generate_for_schema(schema);
	}

	return 0;
}
