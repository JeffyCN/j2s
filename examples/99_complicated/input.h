#include <stdio.h>
#include <stdint.h>


typedef int test_type;
typedef int array_4x1_int[4];

typedef enum {
   ERR_CODE_1 = 1,
   ERR_CODE_2,
   ERR_CODE_3,
} error_code_t;

/* test0 */
// test1
typedef struct abc {
	char name[20];
	char *desc;
	char **str;
	int str_len;
	char *names[20];
	int int_a; // test2
	long long long_a; /* test3 */
	/* test4 */array_4x1_int *array_ptr;
	int *ptr_a;
	int ptr_a_len;
	int /* test5 */ array_ptr_len;
	error_code_t code;
} param_a /* test6 */;

typedef struct {
	char name[20];
	union {
		int int_b[4]; /* @desc: desc1="int_b" */
		int64_t int64_b;
	};
	param_a param_a; // @desc:type=param_a_xxx desc2=1 desc3=param_a
	/*
	 * test8
	 * */
} param_b;

typedef  struct   {
	char name[20];
	/* @desc: desc4=double_c */
	double double_c;
	param_a param_a;
	/* @desc: desc5=test
	 * @desc: desc6=param_b
	 * */
	param_b param_b;
} param_c;

typedef struct {
	char name[20];
	// @desc: desc7=param_c
	param_c param_c;
	int array[10];
} param_d;

typedef enum {
    JSON_OBJECT = 0,
    JSON_ARRAY,
    JSON_STRING,
    JSON_INTEGER,
    JSON_REAL = 10,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL
} json_type;
