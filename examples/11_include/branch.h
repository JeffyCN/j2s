#include "leaf.h"

typedef leaf_t (*array_ptr_leaf_t)[3];
typedef struct {
	array_ptr_leaf_t leaves;
	int leaves_len;
} branch_t;
