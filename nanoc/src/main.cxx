#include "generator/cxx/cxx_generator.hxx"
#include "generator/generator.hxx"
#include "generator/swift/swift_generator.hxx"
#include "generator/ts/ts_generator.hxx"
#include "message_schema.hxx"
#include "parser/parse_schema.hxx"
#include "resolver/resolve_schemas.hxx"
#include <argparse/argparse.hpp>
#include <iostream>
#include <oneapi/tbb/concurrent_unordered_map.h>
#include <optional>
#include <string>
#include <thread>
#include <vector>

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("nanoc");

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

	oneapi::tbb::concurrent_unordered_map<std::string, ParseResult>
		all_parse_results;
	std::vector<std::thread> threads;
	for (const std::string &input_file_path : input_files) {
		threads.emplace_back([&all_parse_results, input_file_path]() {
			std::optional<ParseResult> parse_result =
				parse_schema_file(input_file_path);
			if (parse_result.has_value()) {
				all_parse_results.insert(
					{parse_result->partial_schema->message_name,
					 parse_result.value()});
			}
		});
	}

	for (auto &t : threads) {
		t.join();
	}

	Generator *generator;
	if (language == "c++") {
		generator = new CxxGenerator;
	} else if (language == "swift") {
		generator = new SwiftGenerator;
	} else if (language == "ts") {
		generator = new TsGenerator;
	} else {
		std::cerr << "Unsupported language: " << language << std::endl;
		return -1;
	}

	std::vector<MessageSchema> all_schemas = resolve_schemas(all_parse_results);
	for (const MessageSchema &schema : all_schemas) {
		generator->generate_for_schema(schema);
	}

	delete generator;

	return 0;
}
