#include "j2s.h"
#include "input.h"

int compare(void *ptr_a, void *ptr_b) {
	return memcmp(ptr_a, ptr_b, sizeof(struct_a));
}

static const char *template_json = R"({"a_e":"ENUM_A_0","@enum":{"enum_a":{"ENUM_A_0":0,"ENUM_A_1":1,"ENUM_A_2":2}}})";

static const char *init_json = R"({"a_e":"ENUM_A_2"})";

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	ctx.dump_desc = false;

	struct_a init_struct = {
		.a_e = ENUM_A_2,
	};
	struct_a test_struct = {0};

	/* Test template JSON */
	ctx.dump_enums = true;
	buf = j2s_dump_template_root_struct(&ctx);
	DASSERT(buf && !strcmp(buf, template_json), goto err);
	free(buf);

	/* Test struct to JSON */
	ctx.dump_enums = false;
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
	buf = R"({"a_e":"ENUM_A_1"})";
	init_struct.a_e = ENUM_A_1;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	/* Test query JSON from struct */
	const char *sub_json = R"({"a_e":"ENUM_A_1"})";
	buf = R"({"a_e":""})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	/* Test invalide values */
	buf = R"({"a_e":"RANDOM"})";
	init_struct.a_e = -1;
	DASSERT(!j2s_modify_struct(&ctx, buf, &test_struct), goto err);
	DASSERT(!compare(&test_struct, &init_struct), goto err);

	sub_json = R"({"a_e":"INVALID"})";
	buf = R"({"a_e":""})";
	buf = j2s_query_struct(&ctx, buf, &test_struct);
	DASSERT(buf && !strcmp(buf, sub_json), goto err);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
