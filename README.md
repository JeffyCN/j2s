# j2s

Simple APIs for converting between JSON and C structs, based on cJSON <https://sourceforge.net/p/cjson/wiki/Home>

## Table of Contents

I. How it Work

II. Limits

III. How to use it

IV. Examples

V. Tests

### I. How it Work

First of all, there's a simple pre-parser tool to parse the pre-compiled C struct headers and generate a C source to describe the struct members' information table, e.g. name, type, offset, size.

Then the J2S framework can use cJSON to parse JSON and do the conversion based on that information table.

### II. Limits

The parser only supports a few data types, such as char, int, short, long, float, double, struct, enum, union...etc.

The enum and struct defination should use typedef:

```c
typedef enum {
	...
} xxx;
typedef struct {
	...
} xxx;
```

The system C headers should be included at the top of input C header, which would be stripped during parsing.

For arrays, only support array, 2D array, array pointer. And the array pointer defination should be:

```c
typedef <type> (*array_ptr_<type>)[len]
```

For pointer member "data"(including array pointer), requires an extra int member "data_len" to describe the allocated data length.

The float(or double)'s decimal places of precision limited to 6:
```c
// src/cJSON.c
#if 0
	...
#else
	/* HACK: Force use 6 decimal places of precision */
	length = sprintf((char*)number_buffer, "%1.6g", d);
#endif
```

### III. How to use it

There's also a demo to show how to use this:
```shell
# Run demo
$ ./demo.sh
```

### IV. Examples

There're a few examples under examples/:
```shell
# Run all examples
$ ./example.sh

# Run single example
$ ./example.sh examples/01_struct_to_json/
```

### V. Tests

There're also a few tests under tests/:
```shell
# Run all tests
$ ./test.sh

# Run single test
$ ./test.sh tests/01_basic/
```

## Maintainers

* Jeffy Chen `<jeffy.chen@rock-chips.com>`
