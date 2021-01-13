#include <math.h>

#include "j2s.h"
#include "input.h"

int compare(void *ptr_a, void *ptr_b) {
	struct_c *a = ptr_a;
	struct_c *b = ptr_b;

	if (memcmp(ptr_a, ptr_b, (uintptr_t)&((struct_c *)0)->c_d))
		return -1;

	if (fabs(a->c_d - b->c_d) >= 0.000001)
		return -1;

	if (fabs(a->c_f - b->c_f) >= 0.000001)
		return -1;

	return 0;
}

static const char *template_json = R"({"c_b":{"a_i32":0,"a_i64":0,"b_a":{"a_i8":0,"a_i16":0}},"u_b":0,"u_c":0,"c_d":0,"c_f":0})";

static const char *init_json = R"({"c_b":{"a_i32":32,"a_i64":64,"b_a":{"a_i8":8,"a_i16":16}},"u_b":1,"u_c":1,"c_d":2.2,"c_f":3.3})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	struct_c init_struct = {
		.c_b = {
			.a_i32 = 32,
			.a_i64 = 64,
			.b_a = {
				.a_i8 = 8,
				.a_i16 = 16,
			},
		},
		.u_b = 1,
		.c_d = 2.2,
		.c_f = 3.3,
	};

	struct_c test_struct = {0};

	/* Test template JSON */
	buf = j2s_dump_template_root_struct(&ctx);
	DASSERT(buf && !strcmp(buf, template_json), goto err);
	free(buf);

	/* Test struct to JSON */
	buf = j2s_dump_root_struct(&ctx, &init_struct);
	DASSERT(buf && !strcmp(buf, init_json), goto err);
	free(buf);

	/* Test JSON file to struct */
	DASSERT(!j2s_json_file_to_struct(&ctx, argv[1], &test_struct),
		goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test struct cache */
	DASSERT(!j2s_json_file_to_struct(&ctx, argv[1], &test_struct),
		goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	char cache[] = "/tmp/.j2s-cache";
	j2s_save_cache(&ctx, cache, &test_struct);
	memset(&test_struct, 0, sizeof(test_struct));
	DASSERT(!j2s_load_cache(&ctx, cache, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test apply JSON to struct */
	buf = R"({"c_b":{"a_i32":23,"b_a":{"a_i16":61}},"u_b":0,"c_d":4.4,"c_f":5.5})";
	init_struct.u_b = 0;
	init_struct.c_b.a_i32 = 23;
	init_struct.c_b.b_a.a_i16 = 61;
	init_struct.c_d = 4.4;
	init_struct.c_f = 5.5;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"c_b":{"a_i32":23,"b_a":{"a_i16":61}},"u_b":0,"c_d":4.4,"c_f":5.5})";
	buf = R"({"c_b":{"a_i32":0,"b_a":{"a_i16":0}},"u_b":0,"c_d":0,"c_f":0})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
