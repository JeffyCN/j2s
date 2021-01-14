#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;
	//ctx.manage_data = false;

	const char *json = R"(
{
        "branches":     [{
                        "leaves":       [{
                                        "color":        "GREEN"
                                }, {
                                        "color":        "RED"
                                }]
                }, {
                        "leaves":       [{
                                        "color":        "YELLOW"
                                }, {
                                        "color":        "RED"
                                }, {
                                        "color":        "GREEN"
                                }]
                }]
}
	)";
	tree_t tree = {0};

	printf("Apply JSON:\n%s\n", json);
	DASSERT(!j2s_modify_struct(&ctx, json, &tree), goto err);

	/* Verify */
	buf = j2s_dump_root_struct(&ctx, &tree);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
