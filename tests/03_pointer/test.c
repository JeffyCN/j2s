#include "j2s.h"
#include "input.h"

int compare(void *ptr_a, void *ptr_b) {
	struct_b *a = ptr_a;
	struct_b *b = ptr_b;

	if (a->b_a_len != b->b_a_len)
		return -1;

	for (int i = 0; i < a->b_a_len; i++) {
		struct_a *a_a = &a->b_a[i];
		struct_a *b_a = &b->b_a[i];

		if (a_a->a_i_len != b_a->a_i_len)
			return -1;

		if (memcmp(a_a->a_i, b_a->a_i, a_a->a_i_len) < 0)
			return -1;
	}

	return 0;
}

static const char *template_json = R"({"b_a":[{"a_i":[0],"a_i_len":1}],"b_a_len":1})";

static const char *init_json = R"({"b_a":[{"a_i":[1,2],"a_i_len":2},{"a_i":[3,4],"a_i_len":2}],"b_a_len":2})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	struct_b init_struct = {0};
	int *a_i[2];
	a_i[0]= malloc(2 * sizeof(int));
	a_i[0][0] = 1;
	a_i[0][1] = 2;
	a_i[1]= malloc(2 * sizeof(int));
	a_i[1][0] = 3;
	a_i[1][1] = 4;
	struct_a *a = malloc(2 * sizeof(*a));
	a[0].a_i = a_i[0];
	a[0].a_i_len = 2;
	a[1].a_i = a_i[1];
	a[1].a_i_len = 2;
	init_struct.b_a = a;
	init_struct.b_a_len = 2;

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
	buf = R"({"b_a":[{"a_i":[4,5,6],"a_i_len":3}],"b_a_len":1})";
	free(a_i[0]);
	free(a_i[1]);
	a[0].a_i = malloc(3 * sizeof(int));
	a[0].a_i[0] = 4;
	a[0].a_i[1] = 5;
	a[0].a_i[2] = 6;
	a[0].a_i_len = 3;
	init_struct.b_a_len = 1;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"b_a_len":1,"b_a":[{"a_i":[4,5,6],"a_i_len":3}]})";
	buf = R"({"b_a":[]})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
