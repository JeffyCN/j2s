typedef struct {
	int a_i;
} struct_a;

typedef int (*array_ptr_int)[2];
typedef struct_a (*array_ptr_struct_a)[2];
typedef struct {
	array_ptr_int b_ip;
	int b_ip_len;
	array_ptr_struct_a b_ap;
	int b_ap_len;
} struct_b;
