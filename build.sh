#!/bin/sh

# error when there are unset variables instead of silently ignoring
set -eu

# make sure the cwd is where the script is.
pushd "$(dirname "$0")" >> /dev/null

for arg in "$@"; do declare $arg=1; done

# use clang by default unless gcc is specified
if [ ! -v clang ]; then gcc=1; fi
# do release build unless debug is specified
if [ ! -v debug ]; then release=1; fi

compiler="${CC:-g++}"
if [ -v clang ]; then compiler="clang"; fi
if [ -v g++ ]; then	compiler="g++"; fi

ar="${AR:-ar}"

if [ -v debug ]; then
	echo "building in debug mode."
fi
if [ -v release ]; then
	echo "building in release mode."
fi
echo "using ${compiler}."

src_files=(
	"src/any.cxx"
	"src/reader.cxx" 
	"src/writer.cxx" 
	"src/rpc/in_memory_channel.cxx"
	"src/rpc/rpc_client.cxx"
  "src/rpc/rpc_client_channel.cxx"
  "src/rpc/rpc_server.cxx"
	"src/rpc/rpc_server_channel.cxx"
  "src/rpc/standard_io_channel.cxx"
)

clang_common_opts="-I../include -Wall --std=c++20"
clang_debug="$compiler --debug --optimize -DDEBUG ${clang_common_opts}"
clang_release="$compiler --optimize -DDEBUG=0 ${clang_common_opts}"

gcc_common_opts="-I../include -Wall --std=c++20"
gcc_debug="$compiler --debug --optimize -DDEBUG ${gcc_common_opts}"
gcc_release="$compiler --optimize -DDEBUG=0 ${gcc_common_opts}"

if [ -v clang ]; then
	compile_debug="$clang_debug"
	compile_release="$clang_release"
fi
if [ -v gcc ]; then
	compile_debug="$gcc_debug"
	compile_release="$gcc_release"
fi

if [ -v debug ]; then
	compile="$compile_debug"
fi
if [ -v release ]; then
	compile="$compile_release"
fi

mkdir -p build

cd build

all_src=""
for p in "${src_files[@]}"; do
	src="../${p}"
	out="$(basename ${src}).o"
	$compile -c $src -o $out
done

$ar -rcs libnanopack.a *.o
rm *.o

popd >> /dev/null

