#!/bin/sh -e

INPUT=${1:-input.h}

EXTRA_CFLAGS="$EXTRA_CFLAGS -I${INPUT%/*}"

# Precompile input header and keeping the comments
gcc $EXTRA_CFLAGS $INPUT -E -C -o .output.h

# Drop all global headers
cat .output.h |tac |sed '/^#.*"\/usr\//,$d' |tac |grep -v "^#" > output.h

# Compile parser tool
COMMON_SRC="tools/parser.c"
COMMON_CFLAGS="$CFLAGS $EXTRA_CFLAGS \
	-D_GNU_SOURCE \
	-Iinclude \
	-g -O0 \
"
gcc $COMMON_CFLAGS $COMMON_SRC -o parser

# Generate parsed C header
{
	echo "#include \"$(realpath --relative-to=. $INPUT)\""
	./parser output.h
} > j2s_generated.h
