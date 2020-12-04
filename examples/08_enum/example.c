#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;

	leaf_t leaf = {
		.color = RED,
	};

	printf("template JSON with enum\n");
	buf = j2s_dump_template_root_struct(&ctx);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	const char *json = R"(
{
        "color": "YELLOW"
}
	)";

	printf("Apply JSON:\n%s\n", json);
	DASSERT(!j2s_modify_struct(&ctx, json, &leaf), goto err);

	/* Verify */
	buf = j2s_dump_root_struct(&ctx, &leaf);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
