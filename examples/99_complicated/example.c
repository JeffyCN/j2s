#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	param_d param = {};
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;

	/* Initialize struct */
	sprintf(param.name, "init_d");
	sprintf(param.param_c.name, "init_c");
	param.param_c.double_c = 2.2;
	param.param_c.param_a.int_a = 4;
	param.param_c.param_b.param_a.int_a = 3;

	/* Dump root struct to JSON */
	printf("JSON from initial struct:\n");
	buf = j2s_dump_root_struct(&ctx, &param);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	/* Dump sub struct to JSON */
	printf("JSON from sub struct:\n");
	buf = j2s_dump_struct(&ctx, "param_b", &param.param_c.param_b);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	if (argc > 1) {
		/* Apply JSON file to struct */
		DASSERT(!j2s_json_file_to_struct(&ctx, argv[1], &param),
			goto err);
		buf = j2s_dump_root_struct(&ctx, &param);
		DASSERT(buf, goto err);
		printf("Result:\n%s\n", buf);
		free(buf);
	}

	/* Apply JSON to struct */
	buf = "{ \"param_c\": { \"param_b\": { \"param_a\": { \"name\": \"mod_param_a\", \"ptr_a\": [10, 11], \"code\": \"ERR_CODE_3\" }}}, \"array\": [5, 4, 3, 2, 1], \"@array_index\": [1, 2, 3, 4, 5] }";
	printf("Modify with:\n%s\n", buf);
	DASSERT(!j2s_modify_struct(&ctx, buf, &param), goto err);
	buf = j2s_dump_root_struct(&ctx, &param);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	/* Query JSON from struct */
	buf = "{ \"name\": \"123\", \"param_c\": { \"param_b\": { \"param_a\": { \"name\": \"123\" }}}, \"array\": [], \"@array_index\": [3, 4, 5] }";
	printf("Query sub:\n%s\n", buf);
	buf = j2s_query_struct(&ctx, buf, &param);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
