#include "j2s.h"
#include "input.h"

int compare(void *ptr_a, void *ptr_b) {
	return memcmp(ptr_a, ptr_b, sizeof(struct_b));
}

static const char *template_json = R"({"b_i":[0,0],"b_is":[[0,0,0],[0,0,0]],"b_a":[{"a_i":0},{"a_i":0}],"b_as":[[{"a_i":0},{"a_i":0}]]})";

static const char *init_json = R"({"b_i":[1,2],"b_is":[[3,4,5],[6,7,8]],"b_a":[{"a_i":1},{"a_i":2}],"b_as":[[{"a_i":3},{"a_i":4}]]})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	struct_b init_struct = {
		.b_i = {1, 2},
		.b_is = {{3, 4, 5}, {6, 7, 8}},
		.b_a = {
			{ .a_i = 1 },
			{ .a_i = 2 },
		},
		.b_as = {{
			{ .a_i = 3 },
			{ .a_i = 4 },
		}},
	};

	struct_b test_struct = {0};

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

	/* Test apply JSON to struct */
	buf = R"({"b_i":[2,1],"b_is":[[],[8,7,6]],"b_as":[[{},{"a_i":6}]]})";
	init_struct.b_i[0] = 2;
	init_struct.b_i[1] = 1;
	init_struct.b_is[1][0] = 8;
	init_struct.b_is[1][1] = 7;
	init_struct.b_is[1][2] = 6;
	init_struct.b_as[0][1].a_i = 6;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"b_i":[2,1],"b_is":[[],[8,7,6]],"b_as":[[{},{"a_i":6}]]})";
	buf = R"({"b_i":[0,0],"b_is":[[],[0,0,0]],"b_as":[[{},{"a_i":0}]]})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
