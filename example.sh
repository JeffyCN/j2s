#!/bin/sh -e

DIRS=${@:-$(find examples/* -type d)}

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

	if [ -f $DIR/input.h.in ]; then
		cd $DIR
		cat *.m4 input.h.in | PATH=$PWD:$PATH m4 -E - > .input.h
		cd -
		./parser.sh $DIR/.input.h
	else
		./parser.sh $DIR/input.h
	fi

	./dump.sh

	COMMON_SRC="src/*.c"
	COMMON_CFLAGS="$CFLAGS $EXTRA_CFLAGS \
		-Iinclude -I. \
		-g -O0 \
	"
	$CC $COMMON_CFLAGS $COMMON_SRC $DIR/*.c -o $DIR/run

	./$DIR/run $DIR/input.json

	echo "\033[32m Finished $DIR...\033[0m"
done
