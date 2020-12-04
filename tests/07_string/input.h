typedef char (*array_ptr_char)[20];

typedef struct {
	// test1
	/* test2 */
	/* test3 */char *a_p; /* test4 */ // test5 
	char **a_pp; //test6
	int a_pp_len; /* test7
	test8
	test9 */ // test10
	char a_a[20];
	char a_aa[2][20];
	char *a_pa[2];
	array_ptr_char a_ap;
	int a_ap_len;
} struct_a;
