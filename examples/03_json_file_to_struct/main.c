#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;

	DASSERT_MSG(argc > 1, exit(-1), "usage: %s <JSON file>\n", argv[0]);

	school_t school = {0};

	printf("Apply JSON file:\n%s\n", argv[1]);
	DASSERT(!j2s_json_file_to_struct(&ctx, argv[1], &school), goto err);

	/* Verify */
	buf = j2s_dump_root_struct(&ctx, &school);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
