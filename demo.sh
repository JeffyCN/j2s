#!/bin/sh -e

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

./clean.sh

./parser.sh demo/input.h # Generate parsed header

COMMON_CFLAGS="$CFLAGS $EXTRA_CFLAGS \
        -Iinclude -Igenerated -I. \
        -g -O0 \
        "

$CC $COMMON_CFLAGS src/*.c demo/*.c -Idemo/ -o demo/run

./demo/run
