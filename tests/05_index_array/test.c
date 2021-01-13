#include "j2s.h"
#include "input.h"

int compare(void *ptr_a, void *ptr_b) {
	struct_a *a = ptr_a;
	struct_a *b = ptr_b;

	if (a->a_ip_len != b->a_ip_len)
		return -1;

	if (memcmp(a->a_ip, b->a_ip, a->a_ip_len))
		return -1;

	if (memcmp(a->a_ia, b->a_ia, sizeof(a->a_ia)))
		return -1;

	return 0;
}

static const char *template_json = R"({"a_ip":[0],"a_ip_len":1,"a_ia":[0,0,0,0,0,0,0,0,0,0]})";

static const char *init_json = R"({"a_ip":[0,1,2,3,4],"a_ip_len":5,"a_ia":[9,8,7,6,5,4,3,2,1,0]})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	int *a_ip = malloc(5 * sizeof(int));
	a_ip[0] = 0;
	a_ip[1] = 1;
	a_ip[2] = 2;
	a_ip[3] = 3;
	a_ip[4] = 4;
	struct_a init_struct = {
		.a_ip = a_ip,
		.a_ip_len = 5,
		.a_ia = { 9, 8, 7, 6, 5, 4, 3, 2, 1, 0 },
	};
	struct_a test_struct = {0};

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
	buf = R"({"@a_ip_index":[2,3],"a_ip":[3,2],"@a_ia_index":[5,4,3,2,1],"a_ia":[1,2,3,4,5]})";
	init_struct.a_ip[2] = 3;
	init_struct.a_ip[3] = 2;
	init_struct.a_ia[5] = 1;
	init_struct.a_ia[4] = 2;
	init_struct.a_ia[3] = 3;
	init_struct.a_ia[2] = 4;
	init_struct.a_ia[1] = 5;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	buf = j2s_dump_root_struct(&ctx, &init_struct);
	buf = j2s_dump_root_struct(&ctx, &test_struct);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"@a_ip_index":[2,3],"a_ip":[3,2],"@a_ia_index":[5,4,3,2,1],"a_ia":[1,2,3,4,5]})";
	buf = R"({"@a_ip_index":[2,3],"a_ip":[],"@a_ia_index":[5,4,3,2,1],"a_ia":[]})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
