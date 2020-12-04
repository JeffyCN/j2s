#include "j2s.h"
#include "input.h"

int main(int argc, char** argv) {
	j2s_ctx ctx;
	char *buf;
	int ret = -1;

	j2s_init(&ctx);
	ctx.format_json = false;

	school_t school = {
		.name = strdup("school_a"),
		.classes = {
			{
				.name = strdup("class_a"),
				.groups = {
					{
						.name = strdup("group_a"),
						.students = {
							{
								.name = strdup("s_a"),
							},
							{
								.name = strdup("s_b"),
							},
						}
					},
					{
						.name = strdup("group_b"),
						.students = {
							{
								.name = strdup("s_c"),
							},
							{
								.name = strdup("s_d"),
							},
						}
					},
				}
			},
			{
				.name = strdup("class_b"),
				.groups = {
					{
						.name = strdup("group_c"),
						.students = {
							{
								.name = strdup("s_e"),
							},
							{
								.name = strdup("s_f"),
							},
						}
					},
					{
						.name = strdup("group_d"),
						.students = {
							{
								.name = strdup("s_g"),
							},
							{
								.name = strdup("s_h"),
							},
						}
					},
				}
			},
		},
	};

	const char *json = R"(
{
        "name": "",
        "classes":      [{
                }, {
                        "name": "",
                        "groups":       [{
                                        "name": ""
                                }, {
                                        "students":     [{
                                                }, {
                                                        "name": ""
                                                }]
                                }]
                }]
}
	)";

	printf("Query JSON from struct:\n%s\n", json);
	buf = j2s_query_struct(&ctx, json, &school);
	DASSERT(buf, goto err);
	printf("Result:\n%s\n", buf);
	free(buf);

	ret = 0;
err:
	j2s_deinit(&ctx);
	return ret;
}
