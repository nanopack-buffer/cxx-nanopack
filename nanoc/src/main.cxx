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
	program.add_argument("-f", "--factory-out")
		.help("Specify the folder to which the message factory code file is "
			  "output");

	program.parse_args(argc, argv);

	const auto language = program.get<std::string>("--language");
	const auto input_file_relative_paths =
		program.get<std::vector<std::string>>("input_files");

	if (input_file_relative_paths.empty()) {
		std::cerr << "No input file specified!" << std::endl;
		return -1;
	}

	std::vector<std::filesystem::path> input_file_paths;
	input_file_paths.reserve(input_file_relative_paths.size());
	for (auto &input_file_path_relative : input_file_relative_paths) {
		input_file_paths.emplace_back(
			std::filesystem::canonical(input_file_path_relative));
	}

	oneapi::tbb::concurrent_unordered_map<std::string, ParseResult>
		all_parse_results;
	std::vector<std::thread> threads;
	threads.reserve(input_file_paths.size());
	for (const std::filesystem::path &input_file_path : input_file_paths) {
		threads.emplace_back([&all_parse_results, input_file_path]() {
			if (std::optional<ParseResult> parse_result =
					parse_schema_file(input_file_path);
				parse_result.has_value()) {
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

	const std::vector<MessageSchema> all_schemas =
		resolve_schemas(all_parse_results);
	for (const MessageSchema &schema : all_schemas) {
		generator->generate_for_schema(schema);
	}

	if (std::optional<std::filesystem::path> factory_output_path =
			program.present("--factory-out")) {
		generator->generate_message_factory(all_schemas, *factory_output_path);
	}

	delete generator;

	return 0;
}
