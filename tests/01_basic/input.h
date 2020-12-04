#include <stdbool.h>

#include "test_b.h"

typedef struct { struct_b c_b;
	union { bool u_b; char u_c; }; double c_d;
	float c_f;
} struct_c;
