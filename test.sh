#!/bin/sh -e

unset FAILED_TESTS

DIRS=${@:-$(find tests/* -type d)}

case $ARCH in
	arm)
		CC=arm-linux-gnueabihf-gcc
		;;
	arm64)
		CC=aarch64-linux-gnu-gcc
		;;
	*)
		CC=gcc
		;;
esac

for d in $DIRS;do
	./clean.sh

	DIR=$d

	echo "\033[32m Runing $DIR${ARCH:+ for $ARCH}...\033[0m"

	[ -f $DIR/*env.sh ] && . $DIR/*env.sh

	./parser.sh $DIR/input.h
	./dump.sh

	COMMON_SRC="src/*.c"
	COMMON_CFLAGS="$CFLAGS $EXTRA_CFLAGS \
		-Iinclude -I. \
		-g -O0 \
	"
	$CC $COMMON_CFLAGS $COMMON_SRC $DIR/*.c -o $DIR/run

	if $DIR/run $DIR/input.json; then
		echo "\033[32m $DIR: PASS \033[0m"
	else
		echo "\033[31m $DIR: FAILED \033[0m"
		FAILED_TESTS="$FAILED_TESTS $d"
		[ -n "$TESTS_NO_STOP" ] || break
	fi
done

[ -n "$FAILED_TESTS" ] && echo "\n\033[31m FAILED: $FAILED_TESTS \033[0m"
