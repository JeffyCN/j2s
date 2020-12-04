typedef enum {
	GREEN,
	YELLOW,
	RED,
} leaf_color;

typedef struct {
	leaf_color color;
} leaf_t;

typedef struct {
	leaf_t *leaves;
	int leaves_len;
} branch_t;

typedef struct {
	branch_t *branches;
	int branches_len;
} tree_t;
