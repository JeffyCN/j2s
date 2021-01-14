#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;

	printf("Dump template JSON\n");
	buf = j2s_dump_template_root_struct(&ctx);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
