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

./m4/convert_m4.sh # Convert struct desc
./parser.sh generated/input.h # Generate parsed header
./m4/clean_m4.sh # Clean struct desc
./dump.sh # Dump template JSON header

COMMON_CFLAGS="$CFLAGS $EXTRA_CFLAGS \
	-Iinclude -Igenerated -I. \
	-g -O0 \
	"

$CC $COMMON_CFLAGS -c src/*.c
ar -cr libj2s.a *.o

$CC $COMMON_CFLAGS demo.c -lj2s -L. -o demo

./demo
