# nanopack

NanoPack is a binary serialization format for Poly.
This repo contains the source code for the C++ backing library for NanoPack's generated code,
as well as the source code for `nanoc`, the codegen tool for generating NanoPack code.

## Requirements

- CMake >= 3.25.2
- Conan 2
- An appropriate C++ compiler, e.g. Clang
- Clang-format (`nanoc` formats the generated C++ code with `clang-format`)
    - A flag to change the code formatter may be introduced in the future.

Please follow this guide on [Conan](https://docs.conan.io/2/tutorial/consuming_packages/build_simple_cmake_project.html)
on how to initialize and build a conan 2 project. I will compile the steps and write it down here at some point, but it
is not the focus right now.

## Examples

The `examples/` folder contains some examples on how to define a NanoPack schema, and how to consume the generated code.
