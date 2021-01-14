#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;

	const char *json = R"(
{
        "name": "school_a",
        "classes":      [{
                        "name": "class_a",
                        "groups":       [{
                                        "name": "group_a",
                                        "students":     [{
                                                        "name": "s_a"
                                                }, {
                                                        "name": "s_b"
                                                }]
                                }, {
                                        "name": "group_b",
                                        "students":     [{
                                                        "name": "s_c"
                                                }, {
                                                        "name": "s_d"
                                                }]
                                }]
                }, {
                        "name": "class_b",
                        "groups":       [{
                                        "name": "group_c",
                                        "students":     [{
                                                        "name": "s_e"
                                                }, {
                                                        "name": "s_f"
                                                }]
                                }, {
                                        "name": "group_d",
                                        "students":     [{
                                                        "name": "s_g"
                                                }, {
                                                        "name": "s_h"
                                                }]
                                }]
                }]
}
	)";
	school_t school = {0};

	printf("Apply JSON:\n%s\n", json);
	DASSERT(!j2s_modify_struct(&ctx, json, &school), goto err);

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
