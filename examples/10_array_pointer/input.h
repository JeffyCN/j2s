typedef enum {
	GREEN,
	YELLOW,
	RED,
} leaf_color;

typedef struct {
	leaf_color color;
} leaf_t;

typedef leaf_t (*array_ptr_leaf_t)[3];
typedef struct {
	array_ptr_leaf_t leaves;
	int leaves_len;
} branch_t;

typedef branch_t array_branch_t[2];
typedef struct {
	array_branch_t branches;
} tree_t;
