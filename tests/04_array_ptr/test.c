#include "j2s.h"
#include "input.h"

int compare(void *ptr_a, void *ptr_b) {
	struct_b *a = ptr_a;
	struct_b *b = ptr_b;

	if (a->b_ip_len != b->b_ip_len)
		return -1;

	for (int i = 0; i < a->b_ip_len; i++) {
		if (memcmp(a->b_ip[i], b->b_ip[i], sizeof(*a->b_ip[i])))
			return -1;
	}

	if (a->b_ap_len != b->b_ap_len)
		return -1;

	for (int i = 0; i < a->b_ap_len; i++) {
		if (memcmp(a->b_ap[i], b->b_ap[i], sizeof(*a->b_ap[i])))
			return -1;
	}

	return 0;
}

static const char *template_json = R"({"b_ip":[[0,0]],"b_ip_len":1,"b_ap":[[{"a_i":0},{"a_i":0}]],"b_ap_len":1})";

static const char *init_json = R"({"b_ip":[[1,2],[3,4]],"b_ip_len":2,"b_ap":[[{"a_i":5},{"a_i":6}],[{"a_i":7},{"a_i":8}]],"b_ap_len":2})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	array_ptr_int b_ip = malloc(2 * sizeof(*b_ip));
	b_ip[0][0] = 1;
	b_ip[0][1] = 2;
	b_ip[1][0] = 3;
	b_ip[1][1] = 4;
	array_ptr_struct_a b_ap = malloc(2 * sizeof(*b_ap));
	b_ap[0][0].a_i = 5;
	b_ap[0][1].a_i = 6;
	b_ap[1][0].a_i = 7;
	b_ap[1][1].a_i = 8;
	struct_b init_struct = {
		.b_ip = b_ip,
		.b_ip_len = 2,
		.b_ap = b_ap,
		.b_ap_len = 2,
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
	buf = R"({"b_ip":[[5,6],[7,8],[9,10]],"b_ap":[[{"a_i":1},{"a_i":2}]]})";
	free(b_ip);
	b_ip = malloc(3 * sizeof(*b_ip));
	b_ip[0][0] = 5;
	b_ip[0][1] = 6;
	b_ip[1][0] = 7;
	b_ip[1][1] = 8;
	b_ip[2][0] = 9;
	b_ip[2][1] = 10;
	init_struct.b_ip = b_ip;
	init_struct.b_ip_len = 3;
	b_ap[0][0].a_i = 1;
	b_ap[0][1].a_i = 2;
	init_struct.b_ap_len = 1;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"b_ip_len":3,"b_ip":[[5,6],[7,8],[9,10]],"b_ap_len":1,"b_ap":[[{"a_i":1},{"a_i":2}]]})";
	buf = R"({"b_ip":[],"b_ap":[]})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
