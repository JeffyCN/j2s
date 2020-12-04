#include "j2s.h"
#include ".input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = true;

#ifndef ENABLE_DESC_STRUCT
	school_t school = {0};

	printf("Dump template JSON\n");
	buf = j2s_dump_template_root_struct(&ctx);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);
#else
	printf("JSON from structs(%d)\n", num_info);
	j2s_struct_info *j2s_info = calloc(num_info + 1,
					   sizeof(j2s_struct_info));
	for (int i = 0; i < num_info && info[i].name; i++) {
		j2s_info[i].name = info[i].name;
		j2s_info[i].ptr = info[i].ptr;
	}
	ctx.dump_enums = true;
	buf = j2s_dump_structs(&ctx, j2s_info);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);
#endif

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
