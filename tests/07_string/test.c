#include "j2s.h"
#include "input.h"

static int compare_string(char *a, char *b) {
	if (!a != !b)
		return -1;

	if (!a)
		return 0;

	return strcmp(a, b) ? -1 : 0;
}

int compare(void *ptr_a, void *ptr_b) {
	struct_a *a = ptr_a;
	struct_a *b = ptr_b;

	if (compare_string(a->a_p, b->a_p) < 0)
		return -1;

	if (a->a_pp_len != b->a_pp_len)
		return -1;

	for (int i = 0; i < a->a_pp_len; i++) {
		if (compare_string(a->a_pp[i], b->a_pp[i]) < 0)
			return -1;
	}

	if (strcmp(a->a_a, b->a_a))
		return -1;

	if (strcmp(a->a_aa[0], b->a_aa[0]))
		return -1;

	if (strcmp(a->a_aa[1], b->a_aa[1]))
		return -1;

	if (compare_string(a->a_pa[0], b->a_pa[0]) < 0)
		return -1;

	if (compare_string(a->a_pa[1], b->a_pa[1]) < 0)
		return -1;

	if (a->a_ap_len != b->a_ap_len)
		return -1;

	for (int i = 0; i < a->a_ap_len; i++) {
		if (strcmp(a->a_ap[i], b->a_ap[i]))
			return -1;
	}

	return 0;
}

static const char *template_json = R"({"a_p":"","a_pp":[""],"a_pp_len":1,"a_a":"","a_aa":["",""],"a_pa":["",""],"a_ap":[""],"a_ap_len":1})";

static const char *init_json = R"({"a_p":"a_p","a_pp":["a_pp_0","a_pp_1"],"a_pp_len":2,"a_a":"a_a","a_aa":["a_aa_0","a_aa_1"],"a_pa":["a_pa_0","a_pa_1"],"a_ap":["a_ap_0","a_ap_1"],"a_ap_len":2})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	char **a_pp = malloc(2 * sizeof(*a_pp));
	a_pp[0] = strdup("a_pp_0");
	a_pp[1] = strdup("a_pp_1");
	array_ptr_char a_ap = malloc(2 * sizeof(*a_ap));
	strcpy(a_ap[0], "a_ap_0");
	strcpy(a_ap[1], "a_ap_1");
	struct_a init_struct = {
		.a_p = strdup("a_p"),
		.a_pp = a_pp,
		.a_pp_len = 2,
		.a_pa[0] = strdup("a_pa_0"),
		.a_pa[1] = strdup("a_pa_1"),
		.a_ap = a_ap,
		.a_ap_len = 2,
	};
	strcpy(init_struct.a_a, "a_a");
	strcpy(init_struct.a_aa[0], "a_aa_0");
	strcpy(init_struct.a_aa[1], "a_aa_1");

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
	buf = R"({"a_pp":["a_pp_mod_0"],"a_aa":["a_aa_mod_0"],"a_pa":["a_pa_mod_0"],"a_ap":["a_ap_mod_0","a_ap_mod_1","a_ap_mod_2"],"a_ap_len":3})";
	free(a_pp[0]);
	free(a_pp[1]);
	a_pp[0] = strdup("a_pp_mod_0");
	init_struct.a_pp_len = 1;
	strcpy(init_struct.a_aa[0], "a_aa_mod_0");
	free(init_struct.a_pa[0]);
	init_struct.a_pa[0] = strdup("a_pa_mod_0");
	free(a_ap);
	a_ap = malloc(3 * sizeof(*a_ap));
	init_struct.a_ap = a_ap;
	init_struct.a_ap_len = 3;
	strcpy(a_ap[0], "a_ap_mod_0");
	strcpy(a_ap[1], "a_ap_mod_1");
	strcpy(a_ap[2], "a_ap_mod_2");
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"a_aa":["a_aa_mod_0","a_aa_1"],"a_pa":["a_pa_mod_0","a_pa_1"],"a_pp_len":1,"a_pp":["a_pp_mod_0"],"a_ap_len":3,"a_ap":["a_ap_mod_0","a_ap_mod_1","a_ap_mod_2"]})";
	buf = R"({"a_pp":[],"a_aa":["",""],"a_pa":["",""],"a_ap":[]})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
