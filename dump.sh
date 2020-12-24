#!/bin/sh -e

# Enable this to save spaces
# DUMP_OPTS="--unformat"

# Compile dump tool
COMMON_SRC="src/*.c"
COMMON_CFLAGS="$CFLAGS $EXTRA_CFLAGS \
	-Iinclude -I. \
	-g -O0 \
	-DJ2S_ENABLE_DESC \
"
gcc $COMMON_CFLAGS $COMMON_SRC tools/dump.c -o dump

# Generate template JSON
./dump --template $DUMP_OPTS > .j2s_template.json

# Generate template JSON C header
xxd -iu .j2s_template.json .j2s_template_json.h
{
	echo "#if 0 // Template JSON:"
	cat .j2s_template.json
	echo "#endif"
	echo

	cat .j2s_template_json.h
} > j2s_generated_template_json.h
